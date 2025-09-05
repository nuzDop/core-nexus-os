/* kernel/src/net/arp.c */

#include "arp.h"
#include "ethernet.h"
#include "../lib/string.h"
#include "../mem/pmm.h"

#define ARP_CACHE_SIZE 16
#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY   2

typedef enum {
    ARP_STATE_EMPTY,
    ARP_STATE_RESOLVED,
    ARP_STATE_PENDING
} arp_cache_state_t;

typedef struct {
    arp_cache_state_t state;
    uint32_t ip_addr;
    uint8_t mac_addr[6];
} arp_cache_entry_t;

static arp_cache_entry_t arp_cache[ARP_CACHE_SIZE];

void arp_init() {
    memset(arp_cache, 0, sizeof(arp_cache_entry_t) * ARP_CACHE_SIZE);
    print("ARP Cache initialized.\n");
}

static void arp_update_cache(uint32_t ip, uint8_t* mac) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].state != ARP_STATE_EMPTY && arp_cache[i].ip_addr == ip) {
            memcpy(arp_cache[i].mac_addr, mac, 6);
            arp_cache[i].state = ARP_STATE_RESOLVED;
            return;
        }
    }
    // Find an empty slot
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].state == ARP_STATE_EMPTY) {
            arp_cache[i].ip_addr = ip;
            memcpy(arp_cache[i].mac_addr, mac, 6);
            arp_cache[i].state = ARP_STATE_RESOLVED;
            return;
        }
    }
    // No empty slot, overwrite the first entry (simple replacement)
    arp_cache[0].ip_addr = ip;
    memcpy(arp_cache[0].mac_addr, mac, 6);
    arp_cache[0].state = ARP_STATE_RESOLVED;
}

void arp_handle_packet(net_device_t* dev, uint8_t* data, uint32_t len) {
    if (len < sizeof(arp_packet_t)) return;

    arp_packet_t* arp = (arp_packet_t*)data;
    arp->htype = ntohs(arp->htype);
    arp->ptype = ntohs(arp->ptype);
    arp->oper = ntohs(arp->oper);

    if (arp->htype != 1 || arp->ptype != ETHERTYPE_IP) {
        return; // We only support ARP for IPv4 over Ethernet
    }
    
    // It's a reply to us or a request we should respond to.
    // In either case, we can learn the sender's mapping.
    arp_update_cache(*(uint32_t*)arp->spa, arp->sha);

    if (arp->oper == ARP_OP_REQUEST && *(uint32_t*)arp->tpa == dev->ip_addr) {
        // Someone is asking for our MAC address
        arp_packet_t reply;
        reply.htype = htons(1);
        reply.ptype = htons(ETHERTYPE_IP);
        reply.hlen = 6;
        reply.plen = 4;
        reply.oper = htons(ARP_OP_REPLY);

        memcpy(reply.sha, dev->mac_addr, 6);
        memcpy(reply.spa, (uint8_t*)&dev->ip_addr, 4);
        memcpy(reply.tha, arp->sha, 6);
        memcpy(reply.tpa, arp->spa, 4);

        ethernet_frame_t frame;
        memcpy(frame.dest_mac, arp->sha, 6);
        memcpy(frame.src_mac, dev->mac_addr, 6);
        frame.ethertype = htons(ETHERTYPE_ARP);

        uint8_t* packet = (uint8_t*)pmm_alloc_page();
        memcpy(packet, &frame, sizeof(ethernet_frame_t));
        memcpy(packet + sizeof(ethernet_frame_t), &reply, sizeof(arp_packet_t));

        dev->send_packet(dev, packet, sizeof(ethernet_frame_t) + sizeof(arp_packet_t));
        pmm_free_page(packet);
    }
}

void arp_lookup(net_device_t* dev, uint32_t ip, uint8_t* mac_out) {
    // Check cache first
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].state == ARP_STATE_RESOLVED && arp_cache[i].ip_addr == ip) {
            memcpy(mac_out, arp_cache[i].mac_addr, 6);
            return;
        }
    }

    // Not in cache, send an ARP request
    arp_packet_t request;
    request.htype = htons(1);
    request.ptype = htons(ETHERTYPE_IP);
    request.hlen = 6;
    request.plen = 4;
    request.oper = htons(ARP_OP_REQUEST);

    memcpy(request.sha, dev->mac_addr, 6);
    memcpy(request.spa, (uint8_t*)&dev->ip_addr, 4);
    memset(request.tha, 0, 6); // We don't know the target hardware address
    memcpy(request.tpa, (uint8_t*)&ip, 4);

    ethernet_frame_t frame;
    memset(frame.dest_mac, 0xFF, 6); // Broadcast
    memcpy(frame.src_mac, dev->mac_addr, 6);
    frame.ethertype = htons(ETHERTYPE_ARP);
    
    uint8_t* packet = (uint8_t*)pmm_alloc_page();
    memcpy(packet, &frame, sizeof(ethernet_frame_t));
    memcpy(packet + sizeof(ethernet_frame_t), &request, sizeof(arp_packet_t));

    dev->send_packet(dev, packet, sizeof(ethernet_frame_t) + sizeof(arp_packet_t));
    pmm_free_page(packet);

    // This is a simplified, blocking implementation.
    // A real OS would queue the original IP packet and schedule the process to sleep.
    // For now, we'll just spin for a bit and hope the reply comes in.
    for(int i = 0; i < 100000000; i++) { __asm__ volatile("nop"); }

    // Check cache again
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].state == ARP_STATE_RESOLVED && arp_cache[i].ip_addr == ip) {
            memcpy(mac_out, arp_cache[i].mac_addr, 6);
            return;
        }
    }

    // Resolution failed
    memset(mac_out, 0, 6);
}
