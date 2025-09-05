#include "acpi.h"
#include "../lib/string.h"
#include "../mem/vmm.h"

// This is a placeholder for a function that finds the RSDP.
// In a real OS, this would scan system memory areas like the EBDA.
static void* find_rsdp(void) {
    // For now, we assume it's at a known address for simplicity.
    // This would need to be passed by the bootloader or found via memory scan.
    return NULL;
}

static acpi_sdt_header_t *rsdt;
static rsdp_descriptor_2_0_t *rsdp_2;

void acpi_init(void) {
    void* rsdp_ptr = find_rsdp();
    if (!rsdp_ptr) {
        // print("ACPI: RSDP not found.\n");
        return;
    }

    rsdp_2 = (rsdp_descriptor_2_0_t*)rsdp_ptr;
    rsdt = (acpi_sdt_header_t*)(uintptr_t)rsdp_2->rsdp.rsdt_address;

    // A real implementation would map the ACPI tables into virtual memory here.
    // print("ACPI: Initialized.\n");
}

void* acpi_find_table(const char* signature) {
    if (!rsdt) return NULL;

    int entries = (rsdt->length - sizeof(acpi_sdt_header_t)) / 4;
    uint32_t *pointers = (uint32_t*)(rsdt + 1);

    for (int i = 0; i < entries; i++) {
        acpi_sdt_header_t *h = (acpi_sdt_header_t *)(uintptr_t)pointers[i];
        if (strcmp(h->signature, signature) == 0) {
            return (void*)h;
        }
    }

    return NULL;
}
