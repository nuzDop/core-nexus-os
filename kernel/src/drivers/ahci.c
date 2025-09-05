/* kernel/src/drivers/ahci.c */

#include "ahci.h"
#include "pci.h"
#include "../mem/pmm.h"
#include "../mem/vmm.h"
#include "../lib/string.h"

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3
#define SATA_SIG_ATAPI 0xEB140101 // ATAPI drive
#define SATA_SIG_ATA   0x00000101 // SATA drive
#define SATA_SIG_SEMB  0xC33C0101 // Enclosure management bridge
#define SATA_SIG_PM    0x96690101 // Port multiplier

#define HBA_GHC_AE (1 << 31) // AHCI Enable
#define HBA_GHC_HR (1 << 0)  // HBA Reset

#define HBA_CMD_ST  (1 << 0)  // Start
#define HBA_CMD_FRE (1 << 4)  // FIS Receive Enable
#define HBA_CMD_CR  (1 << 15) // Command List Running
#define HBA_CMD_FR  (1 << 14) // FIS Receive Running

#define ATA_CMD_READ_DMA_EXT  0x25
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_DEV_LBA (1 << 6)

typedef struct {
    hba_mem_t* hba_mem;
    hba_port_t* port;
    int port_num;
    hba_cmd_header_t* cmd_list;
} ahci_device_t;

static hba_mem_t* hba_base_addr;

// Find a free command slot in a port's command list
static int find_cmd_slot(hba_port_t *port) {
    uint32_t slots = (hba_base_addr->cap & 0x0f00) >> 8; // Number of command slots
    for (int i = 0; i < slots; i++) {
        // If bit is not set in both Command Issue and SATA Active, the slot is free
        if (!((port->ci >> i) & 1) && !((port->sact >> i) & 1)) {
            return i;
        }
    }
    return -1;
}

// Rebase a port and initialize its command structures
static void port_rebase(hba_port_t *port) {
    // Stop the command engine
    port->cmd &= ~HBA_CMD_ST;
    port->cmd &= ~HBA_CMD_FRE;
    while((port->cmd & HBA_CMD_CR) || (port->cmd & HBA_CMD_FR));

    // Allocate Command List (1KB)
    void* cmd_list_base = pmm_alloc_page();
    memset(cmd_list_base, 0, PAGE_SIZE);
    port->clb = (uint64_t)cmd_list_base;

    // Allocate FIS Base (256 bytes)
    void* fis_base = pmm_alloc_page();
    memset(fis_base, 0, PAGE_SIZE);
    port->fb = (uint64_t)fis_base;

    hba_cmd_header_t* cmd_header = (hba_cmd_header_t*)cmd_list_base;
    for (int i = 0; i < 32; i++) {
        // 8 PRDT entries per command table, 256 bytes total
        cmd_header[i].prdtl = 8;
        
        void* cmd_table_base = pmm_alloc_page();
        memset(cmd_table_base, 0, PAGE_SIZE);
        cmd_header[i].ctba = (uint64_t)cmd_table_base;
    }

    // Start the command engine
    port->cmd |= HBA_CMD_FRE;
    port->cmd |= HBA_CMD_ST;
}

void ahci_init() {
    uint8_t bus, device, function;
    pci_scan_for_device(0x01, 0x06, &bus, &device, &function);

    uint32_t bar5 = pci_read_config(bus, device, function, 0x24);
    hba_base_addr = (hba_mem_t*)(uint64_t)(bar5 & 0xFFFFFFF0);
    // This physical address needs to be mapped into virtual memory
    
    // Enable AHCI mode
    hba_base_addr->ghc |= HBA_GHC_AE;

    // Reset HBA
    hba_base_addr->ghc |= HBA_GHC_HR;
    while (hba_base_addr->ghc & HBA_GHC_HR);
    hba_base_addr->ghc |= HBA_GHC_AE;

    uint32_t ports_implemented = hba_base_addr->pi;
    for (int i = 0; i < 32; i++) {
        if ((ports_implemented >> i) & 1) {
            uint32_t ssts = hba_base_addr->ports[i].ssts;
            uint8_t det = ssts & 0x0F;
            uint8_t ipm = (ssts >> 8) & 0x0F;

            if (det == HBA_PORT_DET_PRESENT && ipm == HBA_PORT_IPM_ACTIVE) {
                uint32_t sig = hba_base_addr->ports[i].sig;
                if (sig == SATA_SIG_ATA) {
                    print("AHCI: Found SATA drive at port %d\n", i);
                    port_rebase(&hba_base_addr->ports[i]);
                    // Register a VFS node for this drive
                    create_ahci_device_node(i);
                }
            }
        }
    }
}

// Simplified read/write function
bool ahci_transfer(int port_num, uint64_t lba, uint32_t count, bool write, uint8_t* buffer) {
    hba_port_t* port = &hba_base_addr->ports[port_num];
    port->is = (uint32_t)-1; // Clear any pending interrupt flags
    
    int slot = find_cmd_slot(port);
    if (slot == -1) return false;

    hba_cmd_header_t* cmd_header = (hba_cmd_header_t*)port->clb;
    cmd_header += slot;
    cmd_header->cfl = sizeof(fis_reg_h2d_t) / sizeof(uint32_t); // 5
    cmd_header->w = write ? 1 : 0;
    cmd_header->prdtl = 1; // For now, only one PRDT entry

    hba_cmd_tbl_t* cmd_tbl = (hba_cmd_tbl_t*)cmd_header->ctba;
    memset(cmd_tbl, 0, sizeof(hba_cmd_tbl_t) + (cmd_header->prdtl - 1) * sizeof(hba_prdt_entry_t));
    
    // Setup PRDT
    cmd_tbl->prdt_entry[0].dba = (uint64_t)buffer; // Physical address of buffer
    cmd_tbl->prdt_entry[0].dbc = (count * 512) - 1; // Zero-based byte count
    
    // Setup Command FIS
    fis_reg_h2d_t* cmd_fis = (fis_reg_h2d_t*)(&cmd_tbl->cfis);
    cmd_fis->fis_type = FIS_TYPE_REG_H2D;
    cmd_fis->c = 1; // It's a command
    cmd_fis->command = write ? ATA_CMD_WRITE_DMA_EXT : ATA_CMD_READ_DMA_EXT;
    cmd_fis->device = ATA_DEV_LBA;
    
    cmd_fis->lba0 = (uint8_t)lba;
    cmd_fis->lba1 = (uint8_t)(lba >> 8);
    cmd_fis->lba2 = (uint8_t)(lba >> 16);
    cmd_fis->lba3 = (uint8_t)(lba >> 24);
    cmd_fis->lba4 = (uint8_t)(lba >> 32);
    cmd_fis->lba5 = (uint8_t)(lba >> 40);

    cmd_fis->count_low = (uint8_t)count;
    cmd_fis->count_high = (uint8_t)(count >> 8);

    // Issue the command
    port->ci = (1 << slot);

    // Wait for completion (polling)
    while (1) {
        if (!((port->ci >> slot) & 1)) break; // Command is done
        if (port->is & (1 << 30)) { // Task File Error
            print("AHCI: Disk Error!\n");
            return false;
        }
    }
    
    return true;
}

uint32_t ahci_read(fs_node_t* node, uint64_t offset, uint32_t size, uint8_t* buffer) {
    ahci_device_t* dev = (ahci_device_t*)node->device_info;
    uint32_t sector_start = offset / 512;
    uint32_t sector_count = size / 512;
    if(size % 512) sector_count++;

    if(ahci_transfer(dev->port_num, sector_start, sector_count, false, buffer)) {
        return size;
    }
    return 0;
}

uint32_t ahci_write(fs_node_t* node, uint64_t offset, uint32_t size, uint8_t* buffer) {
    ahci_device_t* dev = (ahci_device_t*)node->device_info;
    uint32_t sector_start = offset / 512;
    uint32_t sector_count = size / 512;
    if(size % 512) sector_count++;

    if(ahci_transfer(dev->port_num, sector_start, sector_count, true, buffer)) {
        return size;
    }
    return 0;
}
