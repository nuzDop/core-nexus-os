#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 0x1000
#define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000

// Page Table Entry Flags
#define PTE_PRESENT  (1ULL << 0)
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER     (1ULL << 2)
#define PTE_NX       (1ULL << 63) // No-Execute Bit

#define PAGING_ADDRESS_MASK 0x000FFFFFFFFFF000

typedef uint64_t pte_t; // Page Table Entry
typedef uint64_t pde_t; // Page Directory Entry
typedef uint64_t pdpte_t; // Page Directory Pointer Table Entry
typedef uint64_t pml4e_t; // Page Map Level 4 Entry

typedef pte_t pt_t[512];
typedef pde_t pd_t[512];
typedef pdpte_t pdpt_t[512];
typedef pml4e_t pml4_t[512];

void vmm_init();
void vmm_map_page(pml4_t* pml4, uint64_t virt, uint64_t phys, uint64_t flags);
pml4_t* clone_pml4(pml4_t* src);

extern volatile pml4_t* current_pml4;

#endif
