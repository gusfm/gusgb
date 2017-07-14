#include "mbc1.h"
#include "cart.h"

extern cart_t CART;

static void mbc1_change_bank(void)
{
    unsigned int rom_bank_tmp = CART.mbc.rom_bank % CART.rom.max_bank;
    CART.rom.offset = (unsigned int)(rom_bank_tmp) * 0x4000;
}

void mbc1_write(uint16_t addr, uint8_t val)
{
    if (addr <= 0x1fff) {
        /* Enable/disable external RAM. */
        CART.mbc.ram_on = (val & 0x0f) == 0x0a ? true : false;
    } else if (addr <= 0x3fff) {
        /* Switch between banks 1-31 (value 0 is seen as 1). */
        uint8_t bankl = val & 0x1f;
        if (bankl == 0)
            bankl = 1;
        if (CART.mbc.mode == 0) {
            CART.mbc.rom_bank = (uint8_t)((CART.mbc.rom_bank & 0x60) | bankl);
        } else {
            CART.mbc.rom_bank = bankl;
        }
        mbc1_change_bank();
    } else if (addr <= 0x5fff) {
        if (CART.mbc.mode) {
            /* RAM mode: switch RAM bank 0-3. */
            CART.mbc.ram_bank = val & 3;
            CART.ram.offset = (unsigned int)(CART.mbc.ram_bank) * 0x2000;
        } else {
            /* ROM mode (high 2 bits): switch ROM bank "set" {1-31}-{97-127}. */
            CART.mbc.rom_bank =
                (uint8_t)((CART.mbc.rom_bank & 0x1f) | ((val & 3) << 5));
            mbc1_change_bank();
        }
    } else {
        CART.mbc.mode = val & 1;
    }
}
