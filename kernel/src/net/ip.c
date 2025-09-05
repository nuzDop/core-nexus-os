/* kernel/src/net/ip.c */

#include "ip.h"
#include "ethernet.h"
#include "arp.h"
#include "icmp.h"
#include "tcp.h"
#include "../lib/string.h"
#include "../mem/pmm.h"

static uint16_t ip_ident = 0;

// Calculates the 16-bit one's complement checksum for a buffer.
static uint16_t ip_calculate_checksum(void* addr, int count) {
    uint32_t sum = 0;
    uint16_t* ptr = (uint16_t*)addr;

    while (count > 1) {
        sum += *ptr++;
        count -= 2;
    }

    if (count > 0) {
        sum += *(uint8_t*)ptr;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~((uint16_t)sum);
}

void ip_handle_packet(net_device_t* dev, uint8_t* data, uint32_t len) {
    if (len < sizeof(ip_packet_t)) return;

    ip_packet_t* ip_pkt = (ip_packet_t*)data;
    uint8_t ihl = (ip_pkt->version_ihl & 0x0F) * 4;

    if (ihl < 20) return; // Invalid header length

    // A real implementation would verify the checksum here.

    // Check if the packet is for us
    if (ip_pkt->dest_ip != dev->ip_addr) {
        return; // Not for us, and we don't route
    }
    
    uint8_t* payload = data + ihl;
    uint32_t payload_len = ntohs(ip_pkt->total_length) - ihl;

    switch (ip_pkt->protocol) {
        case IP_PROTOCOL_ICMP:
            icmp_handle_packet(dev, ip_pkt->src_ip, payload, payload_len);
            break;
        case IP_PROTOCOL_TCP:
            // tcp_handle_packet(dev, ip_pkt->src_ip, payload, payload_len);
            break;
        case IP_PROTOCOL_UDP:
            // udp_handle_packet(dev, ip_pkt->src_ip, payload, payload_len);
            break;
        default:
            // Unsupported protocol
            break;
    }
}

void ip_send_packet(uint32_t dest_ip, uint8_t protocol, uint8_t* payload, uint32_t payload_len) {
    net_device_t* dev = get_default_net_device();
    if (!dev) return;

    uint32_t packet_len = sizeof(ip_packet_t) + payload_len;
    ip_packet_t* ip_pkt = (ip_packet_t*)pmm_alloc_page();
    memset(ip_pkt, 0, packet_len);

    // Build the IP header
    ip_pkt->version_ihl = (4 << 4) | 5; // IPv4, 20-byte header
    ip_pkt->total_length = htons(packet_len);
    ip_pkt->identification = htons(ip_ident++);
    ip_pkt->ttl = 64;
    ip_pkt->protocol = protocol;
    ip_pkt->src_ip = dev->ip_addr;
    ip_pkt->dest_ip = dest_ip;

    // Calculate checksum
    ip_pkt->header_checksum = 0;
    ip_pkt->header_checksum = ip_calculate_checksum(ip_pkt, sizeof(ip_packet_t));
    
    // Copy payload
    memcpy((uint8_t*)ip_pkt + sizeof(ip_packet_t), payload, payload_len);

    // Get destination MAC address via ARP
    uint8_t dest_mac[6];
    arp_lookup(dev, dest_ip, dest_mac);
    if (memcmp(dest_mac, "\0\0\0\0\0\0", 6) == 0) {
        print("IP: ARP lookup failed.\n");
        pmm_free_page(ip_pkt);
        return;
    }

    // Build the Ethernet frame
    ethernet_frame_t frame;
    memcpy(frame.dest_mac, dest_mac, 6);
    memcpy(frame.src_mac, dev->mac_addr, 6);
    frame.ethertype = htons(ETHERTYPE_IP);

    // Create the final packet to send
    uint32_t frame_len = sizeof(ethernet_frame_t) + packet_len;
    uint8_t* final_packet = (uint8_t*)pmm_alloc_page();
    memcpy(final_packet, &frame, sizeof(ethernet_frame_t));
    memcpy(final_packet + sizeof(ethernet_frame_t), ip_pkt, packet_len);
    
    // Send it!
    dev->send_packet(dev, final_packet, frame_len);
    
    pmm_free_page(ip_pkt);
    pmm_free_page(final_packet);
}
