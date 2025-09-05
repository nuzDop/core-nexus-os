#ifndef CMOS_H
#define CMOS_H

#include <stdint.h>

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

void get_rtc_time(rtc_time_t *time);

#endif
