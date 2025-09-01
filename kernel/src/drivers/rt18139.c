#include "rtl8139.h"
#include "../net/net.h"
#include "../mem/pmm.h"
#include "../lib/string.h"

void print(char*);

static void rtl8139_send_packet(net_device_t* dev, uint8_t* payload, uint32_t len) {
    print("RTL8139: Pretending to send a packet.\n");
}

void rtl8139_init() {
    net_device_t* rtl_dev = (net_device_t*)pmm_alloc_page();
    strcpy(rtl_dev->name, "eth0");
    
    // Hardcode MAC and IP for now
    rtl_dev->mac_addr[0] = 0xDE;
    rtl_dev->mac_addr[1] = 0xAD;
    rtl_dev->mac_addr[2] = 0xBE;
    rtl_dev->mac_addr[3] = 0xEF;
    rtl_dev->mac_addr[4] = 0x12;
    rtl_dev->mac_addr[5] = 0x34;
    rtl_dev->ip_addr = 0; // Will be set by DHCP later
    rtl_dev->send_packet = &rtl8139_send_packet;

    // Register this device with the net stack
    rtl_dev->next = net_devices;
    net_devices = rtl_dev;
    
    print("RTL8139 network driver initialized and registered as eth0.\n");
}
