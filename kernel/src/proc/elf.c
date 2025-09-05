#include "elf.h"
#include "../mem/vmm.h"
#include "../fs/vfs.h"
#include "../lib/string.h"

uint32_t elf_load(char* path, page_directory_t* dir) {
    fs_node_t* file = finddir_fs(fs_root, path);
    if (!file) return 0;

    elf_header_t header;
    read_fs(file, 0, sizeof(header), (uint8_t*)&header);

    if (header.magic != ELF_MAGIC) return 0;
    
    // Load program headers
    for (uint32_t i = 0; i < header.ph_count; i++) {
        elf_pheader_t pheader;
        read_fs(file, header.ph_offset + i * header.ph_entry_size, sizeof(pheader), (uint8_t*)&pheader);
        
        if (pheader.type == PT_LOAD) {
            // Map pages for this segment
            for (uint32_t j = 0; j < pheader.mem_size; j += PAGE_SIZE) {
                uint32_t* page = pmm_alloc_page();
                map_page(pheader.virt_addr + j, (uint32_t)page, dir);
            }
            read_fs(file, pheader.offset, pheader.file_size, (uint8_t*)pheader.virt_addr);
        }
        
        // NEW: Check for dynamic linker
        if (pheader.type == PT_INTERP) {
            char interpreter_path[64];
            read_fs(file, pheader.offset, pheader.file_size, (uint8_t*)interpreter_path);
            
            // This is a dynamic executable. Load the interpreter instead.
            // A real implementation would pass the original path to the linker.
            return elf_load(interpreter_path, dir);
        }
    }

    return header.entry;
}
