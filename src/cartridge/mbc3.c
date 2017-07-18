#include "mbc3.h"
#include <string.h>
#include "cart.h"

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

typedef struct {
    uint8_t rom_bank;
    uint8_t ram_bank;
    rtc_t rtc_current;
    rtc_t rtc_latched;
} mbc_t;

mbc_t MBC;
extern cart_t CART;

void mbc3_init(void)
{
    MBC.rom_bank = 1;
    MBC.ram_bank = 0;
    memset(&MBC.rtc_current, 0, sizeof(MBC.rtc_current));
    memset(&MBC.rtc_latched, 0, sizeof(MBC.rtc_latched));
}

static void mbc3_change_bank(void)
{
    unsigned int rom_bank_tmp = MBC.rom_bank % CART.rom.max_bank;
    CART.rom.offset = (unsigned int)(rom_bank_tmp) * 0x4000;
}

void mbc3_write(uint16_t addr, uint8_t val)
{
    if (addr <= 0x1fff) {
        /* Enable/disable external RAM. */
        CART.ram.enabled = (val & 0x0f) == 0x0a ? true : false;
    } else if (addr <= 0x3fff) {
        /* Select ROM bank (value 0 is seen as 1). */
        uint8_t bank = val & 0x7f;
        MBC.rom_bank = bank == 0 ? 1 : bank;
        mbc3_change_bank();
    } else if (addr <= 0x5fff) {
        /* Select RAM bank. */
        MBC.ram_bank = val;
    } else {
        /* Latch Clock Data. */
        static uint8_t last = 0xff;
        if (last == 0 && val == 1) {
            MBC.rtc_latched = MBC.rtc_current;
        }
        last = val;
    }
}

uint8_t mbc3_ram_read(uint16_t addr)
{
    size_t pos = CART.ram.offset + (addr & 0x1fff);
    if (CART.ram.enabled && pos < CART.ram.size) {
        uint8_t bank = MBC.ram_bank;
        if (bank <= 7) {
            return CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)];
        } else if (bank <= 0x0c) {
            return MBC.rtc_latched.reg[bank - 8];
        } else {
            return 0xff;
        }
    } else {
        return 0xff;
    }
}

void mbc3_ram_write(uint16_t addr, uint8_t val)
{
    size_t pos = CART.ram.offset + (addr & 0x1fff);
    if (CART.ram.enabled && pos < CART.ram.size) {
        uint8_t bank = MBC.ram_bank;
        if (bank <= 7) {
            CART.ram.bytes[pos] = val;
        } else if (bank <= 0x0c) {
            if (val & 0x40 || MBC.rtc_current.reg[4] & 0x40) {
                /* The Halt Flag is supposed to be set before writing to the RTC
                 * registers. */
                MBC.rtc_current.reg[bank - 8] = val;
            }
            if ((val & 0x40) == 0) {
                /* TODO: save current time, and make the time increase. */
            }
        }
    }
}
