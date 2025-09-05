#include "cmos.h"

// I/O Ports for CMOS/RTC
#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Helper function to read a CMOS register
static uint8_t read_cmos(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

// Helper to convert BCD to binary
static uint8_t bcd_to_bin(uint8_t val) {
    return (val & 0x0F) + ((val / 16) * 10);
}

void get_rtc_time(rtc_time_t *time) {
    // A more robust implementation would check the "update in progress" flag.
    time->second = bcd_to_bin(read_cmos(0x00));
    time->minute = bcd_to_bin(read_cmos(0x02));
    time->hour   = bcd_to_bin(read_cmos(0x04));
    time->day    = bcd_to_bin(read_cmos(0x07));
    time->month  = bcd_to_bin(read_cmos(0x08));
    uint16_t year_short = bcd_to_bin(read_cmos(0x09));
    // This will need a century byte in a real system, but we'll assume 20xx
    time->year   = year_short + 2000;
}
