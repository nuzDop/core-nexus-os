#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>

#define ETHERTYPE_IP  0x0800
#define ETHERTYPE_ARP 0x0806

typedef struct {
    uint8_t  dest_mac[6];
    uint8_t  src_mac[6];
    uint16_t ethertype;
} __attribute__((packed)) ethernet_frame_t;

#endif
