#ifndef UDP_H
#define UDP_H

#include <stdint.h>

typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_packet_t;

#endif
