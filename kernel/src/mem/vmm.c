#include "vmm.h"
#include "../lib/string.h"

void print(char*); // Forward declare
extern void load_pml4(pml4_t*); // Assembly function to load the PML4 into CR3

void vmm_init() {
    // 1. Create the top-level PML4 table
    pml4_t* pml4 = (pml4_t*)pmm_alloc_page();
    memset(pml4, 0, PAGE_SIZE);

    // 2. Create a PDPT
    pdpt_t* pdpt = (pdpt_t*)pmm_alloc_page();
    memset(pdpt, 0, PAGE_SIZE);
    
    // Link the PML4 to the PDPT
    pml4->entries[0] = (uint64_t)pdpt | 3; // Present, R/W

    // 3. Identity-map the first 1GB of physical memory
    // This is done by filling in one PDPT entry, which maps 1GB.
    for (int i = 0; i < 512; i++) {
        // Create a Page Directory for this 2MB region
        page_directory_t* pd = (page_directory_t*)pmm_alloc_page();
        memset(pd, 0, PAGE_SIZE);
        
        // Link the PDPT to this Page Directory
        pdpt->entries[i] = (uint64_t)pd | 3; // Present, R/W
        
        // Fill the page directory with 2MB pages
        for (int j = 0; j < 512; j++) {
            uint64_t phys_addr = (i * 512 + j) * 0x200000; // 2MB pages
            pd->entries[j] = phys_addr | 0x83; // Present, R/W, Huge Page
        }
    }

    // 4. Load the new PML4 into the CR3 register to enable 64-bit paging
    load_pml4(pml4);
    
    print("64-bit VMM initialized and PML4 loaded.\n");
}
