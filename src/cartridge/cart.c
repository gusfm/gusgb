#include "cart.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "mbc1.h"

#define ROM_OFFSET_TITLE 0x134

cart_t CART;

enum cart_type_e {
    CART_ROM_ONLY = 0x00,
    CART_MBC1 = 0x01,
    CART_MBC1_RAM = 0x02,
    CART_MBC1_RAM_BATTERY = 0x03,
    CART_MBC2 = 0x05,
    CART_MBC2_BATTERY = 0x06,
    CART_ROM_RAM = 0x08,
    CART_ROM_RAM_BATTERY = 0x09,
    CART_MMM01 = 0x0B,
    CART_MMM01_SRAM = 0x0C,
    CART_MMM01_SRAM_BATTERY = 0x0D,
    CART_MBC3_TIMER_BATTERY = 0x0F,
    CART_MBC3_TIMER_RAM_BATTERY = 0x10,
    CART_MBC3 = 0x11,
    CART_MBC3_RAM = 0x12,
    CART_MBC3_RAM_BATTERY = 0x13,
    CART_MBC4 = 0x15,
    CART_MBC4_RAM = 0x16,
    CART_MBC4_RAM_BATTERY = 0x17,
    CART_MBC5 = 0x19,
    CART_MBC5_RAM = 0x1A,
    CART_MBC5_RAM_BATTERY = 0x1B,
    CART_MBC5_RUMBLE = 0x1C,
    CART_MBC5_RUMBLE_RAM = 0x1D,
    CART_MBC5_RUMBLE_RAM_BATTERY = 0x1E,
    CART_POCKET_CAMERA = 0x1F,
    CART_BANDAI_TAMA5 = 0xFD,
    CART_HUC3 = 0xFE,
    CART_HUC1_RAM_BATTERY = 0xFF
};

const char *g_rom_types[256] = {
    [CART_ROM_ONLY] = "ROM ONLY",
    [CART_MBC1] = "MBC1",
    [CART_MBC1_RAM] = "MBC1+RAM",
    [CART_MBC1_RAM_BATTERY] = "MBC1+RAM+BATTERY",
    [CART_MBC2] = "MBC2",
    [CART_MBC2_BATTERY] = "MBC2+BATTERY",
    [CART_ROM_RAM] = "ROM+RAM",
    [CART_ROM_RAM_BATTERY] = "ROM+RAM+BATTERY",
    [CART_MMM01] = "MMM01",
    [CART_MMM01_SRAM] = "MMM01+SRAM",
    [CART_MMM01_SRAM_BATTERY] = "MMM01+SRAM+BATTERY",
    [CART_MBC3_TIMER_BATTERY] = "MBC3+TIMER+BATTERY",
    [CART_MBC3_TIMER_RAM_BATTERY] = "MBC3+TIMER+RAM+BATTERY",
    [CART_MBC3] = "MBC3",
    [CART_MBC3_RAM] = "MBC3+RAM",
    [CART_MBC3_RAM_BATTERY] = "MBC3+RAM+BATTERY",
    [CART_MBC4] = "MBC4",
    [CART_MBC4_RAM] = "MBC4+RAM",
    [CART_MBC4_RAM_BATTERY] = "MBC4+RAM+BATTERY",
    [CART_MBC5] = "MBC5",
    [CART_MBC5_RAM] = "MBC5+RAM",
    [CART_MBC5_RAM_BATTERY] = "MBC5+RAM+BATTERY",
    [CART_MBC5_RUMBLE] = "MBC5+RUMBLE",
    [CART_MBC5_RUMBLE_RAM] = "MBC5+RUMBLE+RAM",
    [CART_MBC5_RUMBLE_RAM_BATTERY] = "MBC5+RUMBLE+RAM+BATTERY",
    [CART_POCKET_CAMERA] = "POCKET+CAMERA",
    [CART_BANDAI_TAMA5] = "BANDAI+TAMA5",
    [CART_HUC3] = "HUC3",
    [CART_HUC1_RAM_BATTERY] = "HUC1+RAM+BATTERY"
};

static mbc_write_f cart_get_mbc(uint8_t cart_type)
{
    switch (cart_type) {
        case CART_ROM_ONLY:
        case CART_MBC1:
        case CART_MBC1_RAM:
        case CART_MBC1_RAM_BATTERY:
            return mbc1_write;
        default:
            return NULL;
    }
}

static int cart_load_header(void)
{
    if (CART.rom.size < sizeof(cart_header_t)) {
        fprintf(stderr, "ERROR: rom too small!\n");
        return -1;
    }
    /* Copy header pointer. */
    cart_header_t *header = (cart_header_t *)&CART.rom.bytes[ROM_OFFSET_TITLE];
    CART.rom.header = header;
    printf("Game title: %s\n", header->title);
    printf("CGB: %d\n", cart_is_cgb());
    /* Get cart type. */
    printf("Cartridge type: %s\n", g_rom_types[header->cart_type]);
    CART.mbc.write_func = cart_get_mbc(header->cart_type);
    if (CART.mbc.write_func == NULL) {
        fprintf(stderr, "Cartridge type not supported!\n");
        return -1;
    }
    /* Get ROM size. */
    unsigned int rom_size_tmp =
        (unsigned int)powf(2.0, (float)(5 + (0xf & header->rom_size)));
    printf("ROM size: %hhu = %uKB\n", header->rom_size, rom_size_tmp);
    if (CART.rom.size != rom_size_tmp * 1024) {
        fprintf(stderr, "ROM file size does not equal header ROM size!\n");
        return -1;
    }
    /* Get RAM size. */
    unsigned int ram_size_tmp = header->ram_size;
    if (ram_size_tmp > 0) {
        ram_size_tmp = (unsigned int)powf(4.0, (float)(ram_size_tmp)) / 2;
    }
    CART.ram.size = ram_size_tmp;
    printf("RAM size: %hhu = %uKB\n", header->ram_size, ram_size_tmp);
    return 0;
}

int cart_load(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }
    /* Get file size. */
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    rewind(file);
    /* Init ROM. */
    CART.rom.size = size;
    CART.rom.bytes = malloc(size);
    CART.rom.offset = 0x4000;
    /* Read rom to memory. */
    size_t read_size = fread(CART.rom.bytes, 1, size, file);
    fclose(file);
    if (read_size != size) {
        fprintf(stderr, "ERROR: fread\n");
        return -1;
    }
    /* Read ROM header. */
    int ret = cart_load_header();
    if (ret < 0) {
        cart_unload();
        return -1;
    }
    /* Init RAM. */
    CART.ram.bytes = malloc(CART.ram.size * 1024);
    CART.ram.offset = 0x0000;
    /* Init MBC. */
    CART.mbc.rom_bank = 0;
    CART.mbc.ram_bank = 0;
    CART.mbc.ram_on = false;
    CART.mbc.mode = 0;
    return 0;
}

void cart_unload(void)
{
    free(CART.rom.bytes);
    free(CART.ram.bytes);
}

uint8_t cart_read_rom0(uint16_t addr)
{
    return CART.rom.bytes[addr];
}

uint8_t cart_read_rom1(uint16_t addr)
{
    return CART.rom.bytes[CART.rom.offset + (addr & 0x3fff)];
}

void cart_write_mbc(uint16_t addr, uint8_t val)
{
    CART.mbc.write_func(addr, val);
}

uint8_t cart_read_ram(uint16_t addr)
{
    size_t pos = CART.ram.offset + (addr & 0x1fff);
    if (pos < CART.ram.size) {
        return CART.ram.bytes[CART.ram.offset + (addr & 0x1fff)];
    } else {
        return 0xff;
    }
}

void cart_write_ram(uint16_t addr, uint8_t val)
{
    size_t pos = CART.ram.offset + (addr & 0x1fff);
    if (pos < CART.ram.size) {
        CART.ram.bytes[pos] = val;
    }
}

bool cart_is_cgb(void)
{
    return CART.rom.header->cgb == 0x80;
}
