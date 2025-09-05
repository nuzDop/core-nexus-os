#ifndef PCI_H
#define PCI_H

#include <stdint.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// PCI Configuration Space Header
typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t  revision_id;
    uint8_t  prog_if;
    uint8_t  subclass;
    uint8_t  class_code;
    // ... other header fields
} pci_header_t;

// Function to read from PCI configuration space
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

// Scans the PCI bus for a device matching a given class and subclass
void pci_scan_for_device(uint8_t class_code, uint8_t subclass, uint8_t* out_bus, uint8_t* out_device, uint8_t* out_function);

#endif
