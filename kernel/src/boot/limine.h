#pragma once

#include <stdint.h>

#define LIMINE_MAGIC1 0xc7b1dd30df4c8b88
#define LIMINE_MAGIC2 0x0a82e883a194f07b

struct limine_file {
    uint64_t revision;
    uint64_t address;
    uint64_t size;
    char *path;
    char *cmdline;
    uint32_t media_type;
    uint32_t unused;
    uint32_t tftp_ip;
    uint32_t tftp_port;
    uint32_t partition_index;
    uint32_t mbr_disk_id;
    uint32_t gpt_disk_uuid[4];
    uint32_t gpt_part_uuid[4];
    uint32_t part_uuid[4];
};

struct limine_memmap_entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
};

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES     6

struct limine_memmap_request {
    uint64_t id[4];
    uint64_t revision;
    struct limine_memmap_response *response;
};

struct limine_memmap_response {
    uint64_t revision;
    uint64_t entry_count;
    struct limine_memmap_entry **entries;
};
