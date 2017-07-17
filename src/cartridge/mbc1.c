#include "mbc1.h"
#include "cart.h"

typedef struct {
    uint8_t rom_bank;
    uint8_t ram_bank;
    uint8_t mode;
} mbc_t;

mbc_t MBC;
extern cart_t CART;

void mbc1_init(void)
{
    MBC.rom_bank = 1;
    MBC.ram_bank = 0;
    MBC.mode = 0;
}

static void mbc1_change_bank(void)
{
    unsigned int rom_bank_tmp = MBC.rom_bank % CART.rom.max_bank;
    CART.rom.offset = (unsigned int)(rom_bank_tmp) * 0x4000;
}

void mbc1_write(uint16_t addr, uint8_t val)
{
    if (addr <= 0x1fff) {
        /* Enable/disable external RAM. */
        CART.ram.enabled = (val & 0x0f) == 0x0a ? true : false;
    } else if (addr <= 0x3fff) {
        /* Switch between banks 1-31 (value 0 is seen as 1). */
        uint8_t bankl = val & 0x1f;
        if (bankl == 0)
            bankl = 1;
        if (MBC.mode == 0) {
            MBC.rom_bank = (uint8_t)((MBC.rom_bank & 0x60) | bankl);
        } else {
            MBC.rom_bank = bankl;
        }
        mbc1_change_bank();
    } else if (addr <= 0x5fff) {
        if (MBC.mode) {
            /* RAM mode: switch RAM bank 0-3. */
            MBC.ram_bank = val & 3;
            CART.ram.offset = (unsigned int)(MBC.ram_bank % CART.ram.max_bank) * 0x2000;
        } else {
            /* ROM mode (high 2 bits): switch ROM bank "set" {1-31}-{97-127}. */
            MBC.rom_bank =
                (uint8_t)((MBC.rom_bank & 0x1f) | ((val & 3) << 5));
            mbc1_change_bank();
        }
    } else {
        MBC.mode = val & 1;
    }
}

uint8_t mbc1_ram_read(uint16_t addr)
{
    size_t pos = CART.ram.offset + (addr & 0x1fff);
    if (CART.ram.enabled && pos < CART.ram.size) {
        return CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)];
    } else {
        return 0xff;
    }
}

void mbc1_ram_write(uint16_t addr, uint8_t val)
{
    if (CART.ram.enabled) {
        size_t pos = CART.ram.offset + (addr & 0x1fff);
        if (pos < CART.ram.size) {
            CART.ram.bytes[pos] = val;
        }
    }
}
