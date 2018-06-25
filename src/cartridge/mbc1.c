#include "mbc1.h"
#include "cart.h"

static uint32_t rom_bank;
static uint32_t ram_bank;
static uint8_t mode;
extern cart_t CART;

void mbc1_init(void)
{
    rom_bank = 1;
    ram_bank = 0;
    mode = 0;
}

void mbc1_write(uint16_t addr, uint8_t val)
{
    if (addr <= 0x1fff) {
        /* Enable/disable external RAM. */
        CART.ram.enabled = (val & 0x0f) == 0x0a;
    } else if (addr <= 0x3fff) {
        /* Switch between banks 1-31 (value 0 is seen as 1). */
        uint8_t bankl = val & 0x1f;
        if (bankl == 0)
            bankl = 1;
        if (mode == 0) {
            rom_bank = (uint8_t)((rom_bank & 0x60) | bankl);
        } else {
            rom_bank = bankl;
        }
        CART.rom.offset = (rom_bank % CART.rom.max_bank) << 14;
    } else if (addr <= 0x5fff) {
        if (mode) {
            /* RAM mode: switch RAM bank 0-3. */
            ram_bank = val & 3;
            CART.ram.offset = (ram_bank % CART.ram.max_bank) << 13;
        } else {
            /* ROM mode (high 2 bits): switch ROM bank "set" {1-31}-{97-127}. */
            rom_bank = (rom_bank & 0x1f) | ((val & 3) << 5);
            CART.rom.offset = (rom_bank % CART.rom.max_bank) << 14;
        }
    } else {
        mode = val & 1;
    }
}

uint8_t mbc1_ram_read(uint16_t addr)
{
    if (CART.ram.enabled) {
        return CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)];
    } else {
        return 0xff;
    }
}

void mbc1_ram_write(uint16_t addr, uint8_t val)
{
    if (CART.ram.enabled) {
        CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)] = val;
    }
}
