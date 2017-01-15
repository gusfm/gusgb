#include "cart.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROM_OFFSET_TITLE 0x134

cart_t CART;

enum rom_type_e {
    ROM_PLAIN = 0x00,
    ROM_MBC1 = 0x01,
    ROM_MBC1_RAM = 0x02,
    ROM_MBC1_RAM_BATT = 0x03,
    ROM_MBC2 = 0x05,
    ROM_MBC2_BATTERY = 0x06,
    ROM_RAM = 0x08,
    ROM_RAM_BATTERY = 0x09,
    ROM_MMM01 = 0x0B,
    ROM_MMM01_SRAM = 0x0C,
    ROM_MMM01_SRAM_BATT = 0x0D,
    ROM_MBC3_TIMER_BATT = 0x0F,
    ROM_MBC3_TIMER_RAM_BATT = 0x10,
    ROM_MBC3 = 0x11,
    ROM_MBC3_RAM = 0x12,
    ROM_MBC3_RAM_BATT = 0x13,
    ROM_MBC5 = 0x19,
    ROM_MBC5_RAM = 0x1A,
    ROM_MBC5_RAM_BATT = 0x1B,
    ROM_MBC5_RUMBLE = 0x1C,
    ROM_MBC5_RUMBLE_SRAM = 0x1D,
    ROM_MBC5_RUMBLE_SRAM_BATT = 0x1E,
    ROM_POCKET_CAMERA = 0x1F,
    ROM_BANDAI_TAMA5 = 0xFD,
    ROM_HUDSON_HUC3 = 0xFE,
    ROM_HUDSON_HUC1 = 0xFF,
};

const char *g_rom_types[256] = {
        [ROM_PLAIN] = "ROM PLAIN",
        [ROM_MBC1] = "ROM+MBC1",
        [ROM_MBC1_RAM] = "ROM+MBC1+RAM",
        [ROM_MBC1_RAM_BATT] = "ROM+MBC1+RAM+BATT",
        [ROM_MBC2] = "ROM+MBC2",
        [ROM_MBC2_BATTERY] = "ROM+MBC2+BATTERY",
        [ROM_RAM] = "ROM+RAM",
        [ROM_RAM_BATTERY] = "ROM+RAM+BATTERY",
        [ROM_MMM01] = "ROM+MMM01",
        [ROM_MMM01_SRAM] = "ROM+MMM01+SRAM",
        [ROM_MMM01_SRAM_BATT] = "ROM+MMM01+SRAM+BATT",
        [ROM_MBC3_TIMER_BATT] = "ROM+MBC3+TIMER+BATT",
        [ROM_MBC3_TIMER_RAM_BATT] = "ROM+MBC3+TIMER+RAM+BATT",
        [ROM_MBC3] = "ROM+MBC3",
        [ROM_MBC3_RAM] = "ROM+MBC3+RAM",
        [ROM_MBC3_RAM_BATT] = "ROM+MBC3+RAM+BATT",
        [ROM_MBC5] = "ROM+MBC5",
        [ROM_MBC5_RAM] = "ROM+MBC5+RAM",
        [ROM_MBC5_RAM_BATT] = "ROM+MBC5+RAM+BATT",
        [ROM_MBC5_RUMBLE] = "ROM+MBC5+RUMBLE",
        [ROM_MBC5_RUMBLE_SRAM] = "ROM+MBC5+RUMBLE+SRAM",
        [ROM_MBC5_RUMBLE_SRAM_BATT] = "ROM+MBC5+RUMBLE+SRAM+BATT",
        [ROM_POCKET_CAMERA] = "ROM+POCKET+CAMERA",
        [ROM_BANDAI_TAMA5] = "ROM+BANDAI+TAMA5",
        [ROM_HUDSON_HUC3] = "ROM+HUDSON+HUC3",
        [ROM_HUDSON_HUC1] = "ROM+HUDSON+HUC1",
};

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
    /* Get cart type. */
    printf("Cartridge type: %s\n", g_rom_types[header->cart_type]);
    if (header->cart_type != ROM_PLAIN && header->cart_type != ROM_MBC1) {
        fprintf(stderr, "Only 32KB games with no mappers are supported!\n");
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
        fprintf(stderr, "ERROR: cart_load_header\n");
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
    if (addr <= 0x1fff) {
        /* Enable/disable external RAM. */
        CART.mbc.ram_on = (val & 0x0f) == 0x0a ? true : false;
    } else if (addr <= 0x3fff) {
        /* Switch between banks 1-31 (value 0 is seen as 1). */
        val &= 0x1f;
        if (val == 0)
            val = 1;
        CART.mbc.rom_bank = (uint8_t)((CART.mbc.rom_bank & 0x60) + val);
        CART.rom.offset = (unsigned int)(CART.mbc.rom_bank * 0x4000);
    } else if (addr <= 0x5fff) {
        if (CART.mbc.mode) {
            /* RAM mode: switch RAM bank 0-3. */
            CART.mbc.ram_bank = val & 3;
            CART.ram.offset = (unsigned int)(CART.mbc.ram_bank * 0x2000);
        } else {
            /* ROM mode (high 2 bits): switch ROM bank "set" {1-31}-{97-127}. */
            CART.mbc.rom_bank =
                (uint8_t)((CART.mbc.rom_bank & 0x1f) + ((val & 3) << 5));
            CART.rom.offset = (unsigned int)(CART.mbc.rom_bank * 0x4000);
        }
    } else {
        CART.mbc.mode = val & 1;
    }
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
