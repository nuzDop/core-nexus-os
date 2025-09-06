/*
 * AHCI SATA driver (minimal, boot-ready)
 * - Probes HBA ports, identifies SATA devices
 * - Exposes read/write via VFS fs_node_t hooks
 *
 * This file is self-contained with local shims for missing kernel glue:
 *  - print() macro -> kprintf()
 *  - pmm_alloc_page() wrapper -> pmm_alloc(PAGE_SIZE)
 *  - PAGE_SIZE default (4096) if not defined
 *
 * Assumes:
 *  - PCI enumeration sets up BAR5 for AHCI HBA mapping (not shown here)
 *  - MMIO mapping provides a valid volatile hba_mem_t* (see ahci.h)
 *  - VFS nodes support ->device_info for per-device pointer
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../lib/string.h"

/* ---- Kernel logging shim ---- */
#ifndef HAVE_KPRINTF
extern void kprintf(const char *fmt, ...);
#endif
#ifndef print
#define print kprintf
#endif

/* ---- Paging/PMM shim ---- */
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096u
#endif

/* Try to use the kernel PMM if available; otherwise declare externs. */
#if !defined(HAVE_PMM_ALLOC_PAGE)
extern void *pmm_alloc(size_t size);
static inline void *pmm_alloc_page(void) {
    return pmm_alloc(PAGE_SIZE);
}
#else
extern void *pmm_alloc_page(void);
#endif

/* ---- Local includes ---- */
#include "ahci.h"           /* AHCI register/struct definitions + ahci_device_t */
#include "../mem/vmm.h"      /* If you need to map MMIO; not strictly required here */
#include "../fs/vfs.h"      /* fs_node_t and VFS ops (now includes device_info) */
#include "../lib/byteswap.h"/* if present; otherwise not required */

/* ------------ Utility macros ------------ */
#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* HBA/Port helpers (names match common OSDev layouts) */
static int find_cmd_slot(hba_port_t *port) {
    /* A command slot is free if both SACT and CI bits are zero. */
    uint32_t slots = port->sact | port->ci;
    for (int i = 0; i < 32; i++) {
        if ((slots & (1u << i)) == 0)
            return i;
    }
    return -1;
}

/* Rebase a single port: allocate command list, FIS, and command tables */
static void port_rebase(hba_port_t *port) {
    /* Stop command engine */
    port->cmd &= ~HBA_PxCMD_ST;
    while (port->cmd & HBA_PxCMD_CR) { /* wait until CR clears */ }

    /* Allocate command list (1 page aligned) */
    void *cmd_list_base = pmm_alloc_page();
    memset(cmd_list_base, 0, PAGE_SIZE);
    port->clb  = (uint32_t)(uintptr_t)cmd_list_base;
    port->clbu = 0;

    /* Allocate received FIS (1 page aligned) */
    void *fis_base = pmm_alloc_page();
    memset(fis_base, 0, PAGE_SIZE);
    port->fb  = (uint32_t)(uintptr_t)fis_base;
    port->fbu = 0;

    /* Command tables: one per slot, we use 256 bytes alignment; one page per slot is simplest */
    hba_cmd_header_t *cmd_hdr = (hba_cmd_header_t *)cmd_list_base;
    for (int i = 0; i < 32; i++) {
        cmd_hdr[i].prdtl = 0;
        cmd_hdr[i].cfl   = 0;
        cmd_hdr[i].w     = 0;
        void *cmd_table_base = pmm_alloc_page();
        memset(cmd_table_base, 0, PAGE_SIZE);
        cmd_hdr[i].ctba  = (uint32_t)(uintptr_t)cmd_table_base;
        cmd_hdr[i].ctbau = 0;
    }

    /* Start command engine */
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

/* --------- Simple per-device struct bound to VFS node --------- */
typedef struct ahci_device {
    hba_port_t    *port;
    uint32_t       sector_size; /* logical block size; default 512 */
    uint64_t       num_sectors; /* capacity if known; 0 = unknown */
} ahci_device_t;

/* Extern from platform code: AHCI controller MMIO base */
extern volatile hba_mem_t *ahci_hba;

/* Probe HBA ports and create device nodes under /dev/sdX (integration left to VFS code) */
static void ahci_probe_and_attach(void) {
    if (!ahci_hba) {
        print("AHCI: HBA MMIO base is NULL; controller not initialized\n");
        return;
    }

    uint32_t pi = ahci_hba->pi;
    for (int i = 0; i < 32; i++) {
        if (!(pi & (1u << i))) continue;

        hba_port_t *port = &ahci_hba->ports[i];

        /* Check device type */
        uint32_t ssts = port->ssts;
        uint8_t ipm = (ssts >> 8) & 0x0F;
        uint8_t det = (ssts >> 0) & 0x0F;
        if (det != HBA_PORT_DET_PRESENT || ipm != HBA_PORT_IPM_ACTIVE) {
            continue;
        }

        uint32_t sig = port->sig;
        if (sig != SATA_SIG_ATAPI && sig != SATA_SIG_ATA && sig != SATA_SIG_SEMB && sig != SATA_SIG_PM) {
            continue;
        }

        /* Rebase buffers */
        port_rebase(port);

        /* Allocate device struct */
        ahci_device_t *dev = (ahci_device_t *)pmm_alloc_page();
        memset(dev, 0, sizeof(*dev));
        dev->port = port;
        dev->sector_size = 512;
        dev->num_sectors = 0;

        /* Create/attach VFS node for this device */
        char name[FS_NAME_MAX];
        /* sdX naming: sda, sdb... based on port index */
        name[0] = 's'; name[1] = 'd';
        name[2] = 'a' + (char)i; name[3] = '\0';

        fs_node_t *node = vfs_create_blockdev_node(name);
        if (!node) {
            print("AHCI: failed to create VFS node for port %d\n", i);
            continue;
        }

        node->flags        = FS_BLOCKDEVICE;
        node->length       = 0; /* unknown until identify */
        node->device_info  = dev;
        node->read         = ahci_read;
        node->write        = ahci_write;

        /* Optional: perform IDENTIFY to fill capacity (not fully implemented here) */
        print("AHCI: Attached %s at port %d (sig=0x%08x)\n", name, i, sig);
    }
}

/* -------------- Low-level I/O helpers --------------
 * Minimal PIO via FIS/command list. For brevity and portability, the following
 * read/write implementation is conservative and synchronous.
 */

static int ahci_wait_ready(hba_port_t *port) {
    /* Wait until the port is not busy and not DRQ */
    for (int i = 0; i < 1000000; i++) {
        if ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) == 0)
            return 0;
    }
    return -1;
}

static int ahci_issue_cmd(hba_port_t *port, int slot) {
    port->ci |= (1u << slot);
    for (int i = 0; i < 10000000; i++) {
        if ((port->ci & (1u << slot)) == 0)
            return 0; /* completed */
    }
    return -1; /* timeout */
}

/* Build a READ DMA EXT command (LBA48) */
static int ahci_read_sectors(hba_port_t *port, uint64_t lba, uint32_t count, void *buffer) {
    if (ahci_wait_ready(port) != 0) return -1;

    int slot = find_cmd_slot(port);
    if (slot < 0) return -1;

    hba_cmd_header_t *cmd_hdr = (hba_cmd_header_t *)(uintptr_t)port->clb;
    hba_cmd_header_t *hdr = &cmd_hdr[slot];
    hdr->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t); /* Command FIS length in DWORDS */
    hdr->w   = 0; /* read */
    hdr->prdtl = 1;

    hba_cmd_tbl_t *tbl = (hba_cmd_tbl_t *)(uintptr_t)hdr->ctba;
    memset(tbl, 0, sizeof(hba_cmd_tbl_t));
    tbl->prdt_entry[0].dba  = (uint32_t)(uintptr_t)buffer;
    tbl->prdt_entry[0].dbau = 0;
    tbl->prdt_entry[0].dbc  = (count * 512) - 1; /* byte count-1 */
    tbl->prdt_entry[0].i    = 1;

    fis_reg_h2d_t *fis = (fis_reg_h2d_t *)&tbl->cfis;
    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c = 1;
    fis->command = ATA_CMD_READ_DMA_EX;

    fis->lba0 = (uint8_t)(lba & 0xFF);
    fis->lba1 = (uint8_t)((lba >> 8) & 0xFF);
    fis->lba2 = (uint8_t)((lba >> 16) & 0xFF);
    fis->device = 1 << 6; /* LBA mode */

    fis->lba3 = (uint8_t)((lba >> 24) & 0xFF);
    fis->lba4 = (uint8_t)((lba >> 32) & 0xFF);
    fis->lba5 = (uint8_t)((lba >> 40) & 0xFF);

    fis->countl = (uint8_t)(count & 0xFF);
    fis->counth = (uint8_t)((count >> 8) & 0xFF);

    /* Issue */
    if (ahci_issue_cmd(port, slot) != 0) return -1;
    return 0;
}

/* Build a WRITE DMA EXT command (LBA48) */
static int ahci_write_sectors(hba_port_t *port, uint64_t lba, uint32_t count, const void *buffer) {
    if (ahci_wait_ready(port) != 0) return -1;

    int slot = find_cmd_slot(port);
    if (slot < 0) return -1;

    hba_cmd_header_t *cmd_hdr = (hba_cmd_header_t *)(uintptr_t)port->clb;
    hba_cmd_header_t *hdr = &cmd_hdr[slot];
    hdr->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t); /* Command FIS length in DWORDS */
    hdr->w   = 1; /* write */
    hdr->prdtl = 1;

    hba_cmd_tbl_t *tbl = (hba_cmd_tbl_t *)(uintptr_t)hdr->ctba;
    memset(tbl, 0, sizeof(hba_cmd_tbl_t));
    tbl->prdt_entry[0].dba  = (uint32_t)(uintptr_t)buffer;
    tbl->prdt_entry[0].dbau = 0;
    tbl->prdt_entry[0].dbc  = (count * 512) - 1; /* byte count-1 */
    tbl->prdt_entry[0].i    = 1;

    fis_reg_h2d_t *fis = (fis_reg_h2d_t *)&tbl->cfis;
    fis->fis_type = FIS_TYPE_REG_H2D;
    fis->c = 1;
    fis->command = ATA_CMD_WRITE_DMA_EX;

    fis->lba0 = (uint8_t)(lba & 0xFF);
    fis->lba1 = (uint8_t)((lba >> 8) & 0xFF);
    fis->lba2 = (uint8_t)((lba >> 16) & 0xFF);
    fis->device = 1 << 6; /* LBA mode */

    fis->lba3 = (uint8_t)((lba >> 24) & 0xFF);
    fis->lba4 = (uint8_t)((lba >> 32) & 0xFF);
    fis->lba5 = (uint8_t)((lba >> 40) & 0xFF);

    fis->countl = (uint8_t)(count & 0xFF);
    fis->counth = (uint8_t)((count >> 8) & 0xFF);

    /* Issue */
    if (ahci_issue_cmd(port, slot) != 0) return -1;
    return 0;
}

/* -------------- VFS bindings -------------- */
size_t ahci_read(fs_node_t *node, uint64_t offset, size_t size, void *buffer) {
    if (!node || !buffer) return 0;
    ahci_device_t *dev = (ahci_device_t *)node->device_info;
    if (!dev || !dev->port) return 0;

    uint32_t sector = dev->sector_size ? dev->sector_size : 512;
    uint64_t lba = offset / sector;
    uint32_t count = (uint32_t)((size + sector - 1) / sector);

    /* For simplicity, assume buffer is contiguous and aligned enough */
    if (ahci_read_sectors(dev->port, lba, count, buffer) != 0) {
        return 0;
    }
    return count * sector;
}

size_t ahci_write(fs_node_t *node, uint64_t offset, size_t size, const void *buffer) {
    if (!node || !buffer) return 0;
    ahci_device_t *dev = (ahci_device_t *)node->device_info;
    if (!dev || !dev->port) return 0;

    uint32_t sector = dev->sector_size ? dev->sector_size : 512;
    uint64_t lba = offset / sector;
    uint32_t count = (uint32_t)((size + sector - 1) / sector);

    if (ahci_write_sectors(dev->port, lba, count, buffer) != 0) {
        return 0;
    }
    return count * sector;
}

/* -------------- Driver init entry -------------- */
void ahci_init(void) {
    print("AHCI: initializing\n");
    ahci_probe_and_attach();
    print("AHCI: init done\n");
}

/* -------------- Weak defaults for VFS integration --------------
 * If your VFS does not provide these helpers,
 * you can implement them or wire up node creation elsewhere.
 */
__attribute__((weak))
fs_node_t *vfs_create_blockdev_node(const char *name) {
    /* Fallback stub: if your VFS already has a device filesystem,
       replace this with the actual factory. Returning NULL will
       make probe fail loudly rather than crash. */
    (void)name;
    return NULL;
}
