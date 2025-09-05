#include "vmm.h"
#include "pmm.h"
#include "../lib/string.h"

#define ALIGNED(x) __attribute__((aligned(x)))

static pml4_t kernel_pml4 ALIGNED(PAGE_SIZE);
volatile pml4_t* current_pml4 = NULL;

extern void load_pml4(pml4_t*);

static uint64_t* get_next_level(uint64_t* table, uint16_t index, bool allocate) {
    if (table[index] & PTE_PRESENT) {
        return (uint64_t*)((table[index] & PAGING_ADDRESS_MASK) + KERNEL_VIRTUAL_BASE);
    }
    if (!allocate) return NULL;
    void* new_level_phys = pmm_alloc_page();
    if (!new_level_phys) return NULL;
    memset((void*)((uint64_t)new_level_phys + KERNEL_VIRTUAL_BASE), 0, PAGE_SIZE);
    table[index] = (uint64_t)new_level_phys | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
    return (uint64_t*)((uint64_t)new_level_phys + KERNEL_VIRTUAL_BASE);
}

void vmm_map_page(pml4_t* pml4_virt, uint64_t virt, uint64_t phys, uint64_t flags) {
    uint16_t pml4_index = (virt >> 39) & 0x1FF;
    uint16_t pdpt_index = (virt >> 30) & 0x1FF;
    uint16_t pd_index   = (virt >> 21) & 0x1FF;
    uint16_t pt_index   = (virt >> 12) & 0x1FF;

    uint64_t* pdpt = get_next_level((uint64_t*)pml4_virt, pml4_index, true);
    uint64_t* pd   = get_next_level(pdpt, pdpt_index, true);
    uint64_t* pt   = get_next_level(pd, pd_index, true);
    
    pt[pt_index] = (phys & PAGING_ADDRESS_MASK) | flags;
}

void vmm_init() {
    memset(&kernel_pml4, 0, sizeof(pml4_t));

    uint64_t phys_base = 0; // From bootloader
    uint64_t virt_base = KERNEL_VIRTUAL_BASE;

    // Map first 4GiB of physical memory to the higher half for the kernel
    for (uint64_t i = 0; i < 0x100000000; i += 0x200000) { // Using 2MB huge pages
        // This requires CPU feature support checks
    }

    // Identity map first 2MB for transition
    for (uint64_t i = 0; i < 0x200000; i += PAGE_SIZE) {
        vmm_map_page(&kernel_pml4, i, i, PTE_PRESENT | PTE_WRITABLE | PTE_NX);
    }

    load_pml4((pml4_t*)((uint64_t)&kernel_pml4 - KERNEL_VIRTUAL_BASE));
    current_pml4 = &kernel_pml4;
    print("VMM: 64-bit 4-level paging initialized.\n");
}

pml4_t* clone_pml4(pml4_t* src) {
    // A full COW implementation is required here.
    return NULL;
}
