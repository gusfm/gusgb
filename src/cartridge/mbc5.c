#include "mbc5.h"
#include "cart.h"

static uint32_t rom_bank;
static uint32_t ram_bank;
extern cart_t CART;

void mbc5_init(void)
{
    rom_bank = 0;
    ram_bank = 0;
    CART.rom.offset = 0;
}

void mbc5_write(uint16_t addr, uint8_t val)
{
    switch ((addr >> 12) & 7) {
        case 0:
        case 1:
            /* 0x0000 - 0x1fff: RAM enable */
            CART.ram.enabled = (val & 0x0f) == 0x0a;
            break;
        case 2:
            /* 0x2000 - 0x2fff: Low bits of ROM bank number */
            rom_bank = (rom_bank & 0x100) | val;
            CART.rom.offset = (rom_bank % CART.rom.max_bank) << 14;
            break;
        case 3:
            /* 0x3000 - 0x3fff: High bit of ROM bank number */
            rom_bank = ((uint16_t)(val & 1) << 8) | (rom_bank & 0xff);
            CART.rom.offset = (rom_bank % CART.rom.max_bank) << 14;
            break;
        case 4:
        case 5:
            /* 0x4000 - 0x5fff: RAM bank number */
            ram_bank = val & 0x0f;
            CART.ram.offset = (ram_bank % CART.ram.max_bank) << 13;
            break;
        default:
            printf("%s: invalid address: 0x%04x\n", __func__, addr);
            break;
    }
}

uint8_t mbc5_ram_read(uint16_t addr)
{
    if (CART.ram.enabled) {
        return CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)];
    } else {
        return 0xff;
    }
}

void mbc5_ram_write(uint16_t addr, uint8_t val)
{
    if (CART.ram.enabled) {
        CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)] = val;
    }
}
