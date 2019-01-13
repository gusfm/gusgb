#ifndef __MBC3_H__
#define __MBC3_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    union {
        uint32_t reg[5];
        struct {
            uint32_t sec;
            uint32_t min;
            uint32_t hour;
            uint32_t dayl;
            uint32_t dayh;
        };
    };
} rtc_time_t;

void rtc_print(rtc_time_t *time);

typedef struct {
    rtc_time_t time;
    rtc_time_t latched_time;
    time_t time_last;
} rtc_t;

void mbc3_init(void);
void mbc3_write(uint16_t addr, uint8_t val);
uint8_t mbc3_ram_read(uint16_t addr);
void mbc3_ram_write(uint16_t addr, uint8_t val);
void mbc3_rtc_update(rtc_time_t *time, time_t diff);
int mbc3_rtc_load(FILE *file);
int mbc3_rtc_save(FILE *file);

#endif /* __MBC3_H__ */
