#ifndef ARP_H
#define ARP_H

#include <stdint.h>
#include "net.h"

typedef struct {
    uint16_t htype;
    uint16_t ptype;
    uint8_t  hlen;
    uint8_t  plen;
    uint16_t oper;
    uint8_t  sha[6];
    uint8_t  spa[4];
    uint8_t  tha[6];
    uint8_t  tpa[4];
} __attribute__((packed)) arp_packet_t;

void arp_init();
void arp_handle_packet(net_device_t* dev, uint8_t* data, uint32_t len);
void arp_lookup(net_device_t* dev, uint32_t ip, uint8_t* mac_out);

#endif
