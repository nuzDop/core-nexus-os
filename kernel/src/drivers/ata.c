/* kernel/src/drivers/ata.c */

#include "ata.h"
#include "../lib/string.h" // For inb/outb helpers

// --- ATA I/O Ports (Primary Bus) ---
#define ATA_DATA_REG        0x1F0
#define ATA_ERROR_REG       0x1F1
#define ATA_SECTOR_COUNT_REG 0x1F2
#define ATA_LBA_LOW_REG     0x1F3
#define ATA_LBA_MID_REG     0x1F4
#define ATA_LBA_HIGH_REG    0x1F5
#define ATA_DRIVE_HEAD_REG  0x1F6
#define ATA_STATUS_REG      0x1F7
#define ATA_COMMAND_REG     0x1F7

// --- ATA Status Register Flags ---
#define ATA_SR_BSY          0x80 // Busy
#define ATA_SR_DRDY         0x40 // Drive Ready
#define ATA_SR_DF           0x20 // Drive Write Fault
#define ATA_SR_DSC          0x10 // Drive Seek Complete
#define ATA_SR_DRQ          0x08 // Data Request Ready
#define ATA_SR_CORR         0x04 // Corrected Data
#define ATA_SR_IDX          0x02 // Index
#define ATA_SR_ERR          0x01 // Error

// --- ATA Commands ---
#define ATA_CMD_READ_PIO    0x20
#define ATA_CMD_WRITE_PIO   0x30
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_IDENTIFY    0xEC

// Waits for the BSY (Busy) bit to be cleared.
static void ata_wait_busy() {
    while (inb(ATA_STATUS_REG) & ATA_SR_BSY);
}

// Waits for the DRQ (Data Request Ready) bit to be set.
static void ata_wait_drq() {
    while (!(inb(ATA_STATUS_REG) & ATA_SR_DRQ));
}

void ata_init() {
    // A full implementation would send the IDENTIFY command to both
    // master and slave drives to detect them and get their parameters.
    print("ATA PIO driver initialized.\n");
}

void ata_read_sectors(uint32_t lba, uint8_t count, uint8_t* buffer) {
    ata_wait_busy();

    // Select master drive (0xE0) and set LBA mode
    outb(ATA_DRIVE_HEAD_REG, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT_REG, count);
    outb(ATA_LBA_LOW_REG, (uint8_t)lba);
    outb(ATA_LBA_MID_REG, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH_REG, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND_REG, ATA_CMD_READ_PIO);

    for (int i = 0; i < count; i++) {
        ata_wait_busy();
        ata_wait_drq();

        // Read 256 16-bit words (512 bytes) from the data port
        uint16_t* ptr = (uint16_t*)buffer;
        for (int j = 0; j < 256; j++) {
            ptr[j] = inw(ATA_DATA_REG);
        }
        buffer += 512;
    }
}

void ata_write_sectors(uint32_t lba, uint8_t count, uint8_t* buffer) {
    ata_wait_busy();

    // Select master drive (0xE0) and set LBA mode
    outb(ATA_DRIVE_HEAD_REG, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT_REG, count);
    outb(ATA_LBA_LOW_REG, (uint8_t)lba);
    outb(ATA_LBA_MID_REG, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH_REG, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND_REG, ATA_CMD_WRITE_PIO);

    for (int i = 0; i < count; i++) {
        ata_wait_busy();
        ata_wait_drq();

        // Write 256 16-bit words (512 bytes) to the data port
        uint16_t* ptr = (uint16_t*)buffer;
        for (int j = 0; j < 256; j++) {
            outw(ATA_DATA_REG, ptr[j]);
        }
        buffer += 512;
    }

    // Flush the cache to ensure data is written to the disk platter
    outb(ATA_COMMAND_REG, ATA_CMD_CACHE_FLUSH);
    ata_wait_busy();
}
