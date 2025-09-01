#include "elf.h"
#include "../fs/vfs.h"
#include "../mem/vmm.h"
#include "../lib/string.h"

void print(char*);

uint32_t elf_load(char* path, page_directory_t* dir) {
    fs_node_t* file = fs_root->finddir(fs_root, path);
    if (!file) {
        print("ELF Loader: Cannot find file.\n");
        return 0;
    }

    elf_header_t header;
    read_fs(file, 0, sizeof(header), (uint8_t*)&header);

    if (*(uint32_t*)header.ident != ELF_MAGIC) {
        print("ELF Loader: Invalid ELF magic.\n");
        return 0;
    }

    for (int i = 0; i < header.phnum; i++) {
        elf_program_header_t phdr;
        read_fs(file, header.phoff + i * header.phentsize, sizeof(phdr), (uint8_t*)&phdr);

        if (phdr.type == 1) { // PT_LOAD
            // Allocate physical memory for this segment
            for (uint32_t p = 0; p < phdr.memsz; p += PAGE_SIZE) {
                void* phys_page = pmm_alloc_page();
                map_page(phdr.vaddr + p, (uint32_t)phys_page, dir);
            }
            
            // Read the segment data from the file into the new memory space
            read_fs(file, phdr.offset, phdr.filesz, (uint8_t*)phdr.vaddr);
        }
    }

    return header.entry;
}
