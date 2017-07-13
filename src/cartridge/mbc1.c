#include "mbc1.h"
#include "cart.h"

extern cart_t CART;

void mbc1_write(uint16_t addr, uint8_t val)
{
    if (addr <= 0x1fff) {
        /* Enable/disable external RAM. */
        CART.mbc.ram_on = (val & 0x0f) == 0x0a ? true : false;
    } else if (addr <= 0x3fff) {
        /* Switch between banks 1-31 (value 0 is seen as 1). */
        val &= 0x1f;
        if (val == 0)
            val = 1;
        CART.mbc.rom_bank = (uint8_t)((CART.mbc.rom_bank & 0x60) + val);
        CART.rom.offset = (unsigned int)(CART.mbc.rom_bank) * 0x4000;
    } else if (addr <= 0x5fff) {
        if (CART.mbc.mode) {
            /* RAM mode: switch RAM bank 0-3. */
            CART.mbc.ram_bank = val & 3;
            CART.ram.offset = (unsigned int)(CART.mbc.ram_bank) * 0x2000;
        } else {
            /* ROM mode (high 2 bits): switch ROM bank "set" {1-31}-{97-127}. */
            CART.mbc.rom_bank =
                (uint8_t)((CART.mbc.rom_bank & 0x1f) + ((val & 3) << 5));
            CART.rom.offset = (unsigned int)(CART.mbc.rom_bank) * 0x4000;
        }
    } else {
        CART.mbc.mode = val & 1;
    }
}

