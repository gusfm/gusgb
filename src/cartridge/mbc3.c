#include "mbc3.h"
#include <string.h>
#include "cart.h"

#define MIN_SECS (60)
#define HOUR_SECS (60 * 60)
#define DAY_SECS (60 * 60 * 24)

static uint32_t rom_bank;
static uint32_t ram_bank;
static rtc_t rtc;

extern cart_t CART;

void mbc3_init(void)
{
    rom_bank = 1;
    ram_bank = 0;
}

int mbc3_rtc_load(FILE *file)
{
    if (file) {
        int rv = fread(&rtc, 1, sizeof(rtc), file);
        if (rv != sizeof(rtc)) {
            fprintf(stderr, "RTC not present in save file\n");
            return -1;
        }
    } else {
        memset(&rtc.time, 0, sizeof(rtc.time));
        memset(&rtc.latched_time, 0, sizeof(rtc.latched_time));
        rtc.time_start = time(NULL);
    }
    printf("RTC current: ");
    rtc_print(&rtc.time);
    printf("RTC latched: ");
    rtc_print(&rtc.latched_time);
    return 0;
}

int mbc3_rtc_save(FILE *file)
{
    int rv = fwrite(&rtc, 1, sizeof(rtc), file);
    if (rv != sizeof(rtc)) {
        fprintf(stderr, "Could not save RTC to save file\n");
        return -1;
    }
    return 0;
}

void mbc3_rtc_update(rtc_time_t *time, time_t diff)
{
    time_t sec = (unsigned int)time->sec;
    time_t min = (unsigned int)time->min;
    time_t hour = (unsigned int)time->hour;
    time_t day = (unsigned int)time->dayl + ((time->dayh & 1) << 8);
    time_t old_time =
        sec + (min * MIN_SECS) + (hour * HOUR_SECS) + (day * DAY_SECS);
    time_t new_time = old_time + diff;

    time_t new_day = new_time / (DAY_SECS);
    time_t day_carry = new_day > 0x1ff;
    time->dayl = new_day & 0xff;
    time->dayh =
        (day_carry << 7) | (time->dayh & 0x40) | (new_day & 0x100) >> 8;

    time_t hour_secs = new_time - new_day * DAY_SECS;
    time->hour = (hour_secs) / HOUR_SECS;
    time->min = (hour_secs % HOUR_SECS) / 60;
    time->sec = new_time % 60;
}

void mbc3_write(uint16_t addr, uint8_t val)
{
    if (addr <= 0x1fff) {
        /* Enable/disable external RAM. */
        CART.ram.enabled = (val & 0x0f) == 0x0a ? true : false;
    } else if (addr <= 0x3fff) {
        /* Select ROM bank (value 0 is seen as 1). */
        uint8_t bank = val & 0x7f;
        rom_bank = bank == 0 ? 1 : bank;
        CART.rom.offset = (rom_bank % CART.rom.max_bank) << 14;
    } else if (addr <= 0x5fff) {
        /* Select RAM bank. */
        ram_bank = val;
    } else {
        /* Latch Clock Data. */
        static uint8_t last = 0xff;
        if (last == 0 && val == 1) {
            time_t time_diff = time(NULL) - rtc.time_start;
            mbc3_rtc_update(&rtc.time, time_diff);
            rtc.latched_time = rtc.time;
        }
        last = val;
    }
}

uint8_t mbc3_ram_read(uint16_t addr)
{
    if (CART.ram.enabled) {
        uint8_t bank = ram_bank;
        if (bank <= 7) {
            return CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)];
        } else if (bank <= 0x0c) {
            return rtc.latched_time.reg[bank - 8];
        } else {
            return 0xff;
        }
    } else {
        return 0xff;
    }
}

void mbc3_ram_write(uint16_t addr, uint8_t val)
{
    if (CART.ram.enabled) {
        uint8_t bank = ram_bank;
        if (bank <= 7) {
            CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)] = val;
        } else if (bank <= 0x0c) {
            if (rtc.time.reg[4] & 0x40 || (bank == 0x0c && val & 0x40)) {
                /* The Halt Flag is supposed to be set before writing to the RTC
                 * registers. */
                rtc.time.reg[bank - 8] = val;
                if ((val & 0x40) == 0) {
                    rtc.time_start = time(NULL);
                }
            }
        }
    }
}

void rtc_print(rtc_time_t *time)
{
    unsigned int day = (unsigned int)time->dayl + ((time->dayh & 1) << 8);
    printf("%.2hhu:%.2hhu:%.2hhu %u\n", time->hour, time->min, time->sec, day);
}
