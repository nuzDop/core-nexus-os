#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>
#include "net.h"

typedef struct {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t rest;
} __attribute__((packed)) icmp_packet_t;

void icmp_handle_packet(net_device_t* dev, uint32_t src_ip, uint8_t* data, uint32_t len);

#endif
