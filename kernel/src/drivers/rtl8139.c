#include "rtl8139.h"
#include "../net/net.h"
#include "../mem/pmm.h"
#include "../lib/string.h"
#include "../net/ethernet.h"
#include "../net/arp.h"
#include "../net/ip.h"

void print(char*);

static void rtl8139_send_packet(net_device_t* dev, uint8_t* payload, uint32_t len) {
    // A real implementation would write to the NIC's transmit buffers and registers.
    print("RTL8139: Pretending to send an Ethernet frame.\n");
}

void rtl8139_receive_packet() {
    // This function would be called by the NIC's interrupt handler.
    // It would read the packet from the receive buffer.
    uint8_t* packet_buffer; // = read from NIC buffer
    uint32_t packet_len; // = read from NIC buffer descriptor

    ethernet_frame_t* frame = (ethernet_frame_t*)packet_buffer;
    
    if (frame->ethertype == htons(ETHERTYPE_IP)) {
        ip_handle_packet(net_devices, (uint8_t*)(frame + 1), packet_len - sizeof(ethernet_frame_t));
    } else if (frame->ethertype == htons(ETHERTYPE_ARP)) {
        arp_handle_packet(net_devices, (uint8_t*)(frame + 1), packet_len - sizeof(ethernet_frame_t));
    }
}

void rtl8139_init() {
    net_device_t* rtl_dev = (net_device_t*)pmm_alloc_page();
    strcpy(rtl_dev->name, "eth0");
    
    rtl_dev->mac_addr[0] = 0xDE;
    rtl_dev->mac_addr[1] = 0xAD;
    rtl_dev->mac_addr[2] = 0xBE;
    rtl_dev->mac_addr[3] = 0xEF;
    rtl_dev->mac_addr[4] = 0x12;
    rtl_dev->mac_addr[5] = 0x34;
    rtl_dev->ip_addr = 0; // Set by DHCP or statically
    rtl_dev->send_packet = &rtl8139_send_packet;

    rtl_dev->next = net_devices;
    net_devices = rtl_dev;
    
    print("RTL8139 network driver initialized and registered as eth0.\n");
}
