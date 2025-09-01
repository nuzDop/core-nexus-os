#include "udp.h"
#include "ip.h"
#include "net.h"
#include "../mem/pmm.h"
#include "../lib/string.h"

void print(char*); // Forward declare from main.c

// A simple helper to calculate an internet checksum
uint16_t checksum(void* addr, int count) {
    uint32_t sum = 0;
    uint16_t* ptr = addr;

    while (count > 1) {
        sum += *ptr++;
        count -= 2;
    }
    if (count > 0) {
        sum += *(uint8_t*)ptr;
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}

// A helper for big-endian conversion
uint16_t htons(uint16_t val) {
    return (val >> 8) | (val << 8);
}

void udp_send_packet(uint32_t dest_ip, uint16_t dest_port, uint16_t src_port, void* data, uint32_t len) {
    // 1. Get the network device (assuming only one for now)
    net_device_t* dev = net_devices;
    if (!dev) {
        print("UDP Error: No network device found.\n");
        return;
    }

    // 2. Allocate memory for the full packet (IP header + UDP header + data)
    uint32_t full_packet_size = sizeof(ip_packet_t) + sizeof(udp_packet_t) + len;
    uint8_t* packet_buffer = (uint8_t*)pmm_alloc_page(); // Simplified allocation

    // 3. Fill in the UDP header
    udp_packet_t* udp_header = (udp_packet_t*)(packet_buffer + sizeof(ip_packet_t));
    udp_header->src_port = htons(src_port);
    udp_header->dest_port = htons(dest_port);
    udp_header->length = htons(sizeof(udp_packet_t) + len);
    udp_header->checksum = 0; // Checksum is optional in UDP over IPv4

    // 4. Copy the payload data
    memcpy((void*)udp_header + sizeof(udp_packet_t), data, len);

    // 5. Fill in the IP header
    ip_packet_t* ip_header = (ip_packet_t*)packet_buffer;
    ip_header->version_ihl = (4 << 4) | 5; // IPv4, 5 words (20 bytes) header length
    ip_header->dscp_ecn = 0;
    ip_header->total_length = htons(full_packet_size);
    ip_header->identification = 0; // Not implementing fragmentation
    ip_header->flags_fragment_offset = 0;
    ip_header->ttl = 64;
    ip_header->protocol = 17; // 17 = UDP
    ip_header->src_ip = dev->ip_addr; // Hardcode for now, should be from device
    ip_header->dest_ip = dest_ip;
    ip_header->header_checksum = 0;
    ip_header->header_checksum = checksum(ip_header, sizeof(ip_packet_t));

    // 6. Send the packet to the network driver
    // A real implementation would need an ethernet frame header here.
    dev->send_packet(dev, packet_buffer, full_packet_size);

    pmm_free_page(packet_buffer);
}
