#ifndef IP_H
#define IP_H

#include <stdint.h>
#include "net.h"

#define IP_PROTOCOL_ICMP 1
#define IP_PROTOCOL_TCP  6
#define IP_PROTOCOL_UDP  17

typedef struct {
    uint8_t  version_ihl;
    uint8_t  dscp_ecn;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t header_checksum;
    uint32_t src_ip;
    uint32_t dest_ip;
} __attribute__((packed)) ip_packet_t;

void ip_handle_packet(net_device_t* dev, uint8_t* data, uint32_t len);
void ip_send_packet(uint32_t dest_ip, uint8_t protocol, uint8_t* payload, uint32_t payload_len);

#endif
