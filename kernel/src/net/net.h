#ifndef NET_H
#define NET_H

#include <stdint.h>

// Represents a generic network interface controller (NIC)
typedef struct net_device {
    char name[16];
    uint8_t mac_addr[6];
    uint32_t ip_addr;
    void (*send_packet)(struct net_device* dev, uint8_t* payload, uint32_t len);
    struct net_device* next;
} net_device_t;

// A generic network packet buffer
typedef struct sk_buff {
    uint8_t* data;
    uint32_t len;
} sk_buff_t;

// Initializes the core networking stack.
void net_init();

// A global list of network devices
extern net_device_t* net_devices;

#endif
