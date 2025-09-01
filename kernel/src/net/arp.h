#ifndef ARP_H
#define ARP_H

#include <stdint.h>

typedef struct {
    uint16_t htype;
    uint16_t ptype;
    uint8_t  hlen;
    uint8_t  plen;
    uint16_t oper;
    uint8_t  sha[6]; // Sender hardware address
    uint8_t  spa[4]; // Sender protocol address
    uint8_t  tha[6]; // Target hardware address
    uint8_t  tpa[4]; // Target protocol address
} __attribute__((packed)) arp_packet_t;

#endif
