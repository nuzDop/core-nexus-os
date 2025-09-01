#include "net.h"

// The head of the linked list of network devices in the system
net_device_t* net_devices = 0;

void print(char*); // Forward declare

void net_init() {
    print("Core networking stack initialized.\n");
}
