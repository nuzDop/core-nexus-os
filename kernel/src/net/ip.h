#ifndef IP_H
#define IP_H

#include <stdint.h>

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

#endif
