/* kernel/src/drivers/ahci.h */

#ifndef AHCI_H
#define AHCI_H

#include <stdint.h>
#include "../fs/vfs.h"

// --- AHCI Structure Definitions (as per AHCI 1.3.1 Spec) ---

#define FIS_TYPE_REG_H2D    0x27 // Register FIS - Host to Device
#define FIS_TYPE_REG_D2H    0x34 // Register FIS - Device to Host
#define FIS_TYPE_DMA_ACT    0x39 // DMA Activate FIS - Device to Host
#define FIS_TYPE_DMA_SETUP  0x41 // DMA Setup FIS - Bi-directional
#define FIS_TYPE_DATA       0x46 // Data FIS - Bi-directional
#define FIS_TYPE_BIST       0x58 // BIST Activate FIS - Bi-directional
#define FIS_TYPE_PIO_SETUP  0x5F // PIO Setup FIS - Device to Host
#define FIS_TYPE_DEV_BITS   0xA1 // Set Device Bits FIS - Device to Host

// Frame Information Structure (FIS) for H2D Register Commands
typedef struct {
    uint8_t  fis_type;
    uint8_t  pm_port : 4;
    uint8_t  rsv0 : 3;
    uint8_t  c : 1; // Command bit
    uint8_t  command;
    uint8_t  feature_low;
    uint8_t  lba0, lba1, lba2;
    uint8_t  device;
    uint8_t  lba3, lba4, lba5;
    uint8_t  feature_high;
    uint8_t  count_low, count_high;
    uint8_t  icc;
    uint8_t  control;
    uint8_t  rsv1[4];
} __attribute__((packed)) fis_reg_h2d_t;

// HBA Port Memory Registers (one per port)
typedef volatile struct {
    uint64_t clb;        // 0x00, Command List Base Address
    uint64_t fb;         // 0x08, FIS Base Address
    uint32_t is;         // 0x10, Interrupt Status
    uint32_t ie;         // 0x14, Interrupt Enable
    uint32_t cmd;        // 0x18, Command and Status
    uint32_t rsv0;       // 0x1C, Reserved
    uint32_t tfd;        // 0x20, Task File Data
    uint32_t sig;        // 0x24, Signature
    uint32_t ssts;       // 0x28, SATA Status (SCR0:SStatus)
    uint32_t sctl;       // 0x2C, SATA Control (SCR2:SControl)
    uint32_t serr;       // 0x30, SATA Error (SCR1:SError)
    uint32_t sact;       // 0x34, SATA Active (SCR3:SActive)
    uint32_t ci;         // 0x38, Command Issue
    uint32_t sntf;       // 0x3C, SATA Notification (SCR4:SNotification)
    uint32_t fbs;        // 0x40, FIS-based switch control
    uint32_t rsv1[11];
    uint32_t vendor[4];  // 0x70-0x7F, vendor specific
} __attribute__((packed)) hba_port_t;

// HBA Main Memory Registers (one per controller)
typedef volatile struct {
    uint32_t cap;        // 0x00, Host Capabilities
    uint32_t ghc;        // 0x04, Global Host Control
    uint32_t is;         // 0x08, Interrupt Status
    uint32_t pi;         // 0x0C, Ports Implemented
    uint32_t vs;         // 0x10, Version
    uint32_t ccc_ctl;    // 0x14, Command Completion Coalescing Control
    uint32_t ccc_pts;    // 0x18, Command Completion Coalescing Ports
    uint32_t em_loc;     // 0x1C, Enclosure Management Location
    uint32_t em_ctl;     // 0x20, Enclosure Management Control
    uint32_t cap2;       // 0x24, Host Capabilities Extended
    uint32_t bohc;       // 0x28, BIOS/OS Handoff Control and Status
    uint8_t  rsv[0x60-0x2C];
    uint8_t  vendor[0x9F-0x60+1];
    hba_port_t ports[32]; // 0xA0-0x10FF, Port control registers
} __attribute__((packed)) hba_mem_t;

// Command Header (entry in the Command List)
typedef struct {
    uint8_t  cfl:5;      // Command FIS Length in DWORDS, must be 5 for H2D
    uint8_t  a:1;        // ATAPI
    uint8_t  w:1;        // Write, 1: H2D, 0: D2H
    uint8_t  p:1;        // Prefetchable
    uint8_t  r:1;        // Reset
    uint8_t  b:1;        // BIST
    uint8_t  c:1;        // Clear Busy upon R_OK
    uint8_t  rsv0:1;
    uint8_t  pmp:4;      // Port Multiplier Port
    uint16_t prdtl;      // Physical Region Descriptor Table Length in entries
    volatile uint32_t prdbc; // Physical Region Descriptor Byte Count transferred
    uint64_t ctba;       // Command Table Base Address
    uint32_t rsv1[4];    // Reserved
} __attribute__((packed)) hba_cmd_header_t;

// Physical Region Descriptor Table Entry
typedef struct {
    uint64_t dba;        // Data Base Address
    uint32_t rsv0;
    uint32_t dbc;        // Byte Count, 1-based
} __attribute__((packed)) hba_prdt_entry_t;

// Command Table (pointed to by a Command Header)
typedef struct {
    uint8_t cfis[64];    // Command FIS
    uint8_t acmd[16];    // ATAPI Command, 12 or 16 bytes
    uint8_t rsv[48];
    hba_prdt_entry_t prdt_entry[1]; // Variable-size PRDT (at least 1)
} __attribute__((packed)) hba_cmd_tbl_t;


// --- Public Driver Functions ---
void ahci_init();
fs_node_t* create_ahci_device_node(int port_num);

#endif
