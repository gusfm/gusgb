#ifndef __MBC3_H__
#define __MBC3_H__

#include <stdint.h>
#include <time.h>

typedef struct {
    union {
        uint8_t reg[5];
        struct {
            uint8_t sec;
            uint8_t min;
            uint8_t hour;
            uint8_t dayl;
            uint8_t dayh;
        };
    };
} rtc_t;

void mbc3_init(void);
void mbc3_write(uint16_t addr, uint8_t val);
uint8_t mbc3_ram_read(uint16_t addr);
void mbc3_ram_write(uint16_t addr, uint8_t val);
void mbc3_rtc_update(rtc_t *rtc, time_t diff);

#endif /* __MBC3_H__ */
