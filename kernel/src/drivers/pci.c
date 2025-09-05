#include "pci.h"
#include "../lib/string.h" // For outl/inl

uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)function;

    // Create the configuration address
    address = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunc << 8) | (offset & 0xFC) | 0x80000000);

    // Write the address to the CONFIG_ADDRESS port
    outl(PCI_CONFIG_ADDRESS, address);
    // Read the data from the CONFIG_DATA port
    return inl(PCI_CONFIG_DATA);
}

void pci_scan_for_device(uint8_t class_code, uint8_t subclass, uint8_t* out_bus, uint8_t* out_device, uint8_t* out_function) {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {
                uint32_t vendor_device = pci_read_config(bus, device, function, 0);
                if ((vendor_device & 0xFFFF) == 0xFFFF) { // Invalid device
                    continue;
                }

                uint32_t class_reg = pci_read_config(bus, device, function, 0x08);
                uint8_t base_class = (class_reg >> 24) & 0xFF;
                uint8_t sub_class = (class_reg >> 16) & 0xFF;

                if (base_class == class_code && sub_class == subclass) {
                    *out_bus = bus;
                    *out_device = device;
                    *out_function = function;
                    print("PCI: Found matching device.\n");
                    return;
                }
            }
        }
    }
}
