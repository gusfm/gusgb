#include "cart.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROM_OFFSET_TITLE 0x134

cart_t CART;

const uint8_t g_internal_rom[0x100] = {
    0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb,
    0x21, 0x26, 0xff, 0x0e, 0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3,
    0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0, 0x47, 0x11, 0x04, 0x01,
    0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b,
    0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22,
    0x23, 0x05, 0x20, 0xf9, 0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99,
    0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20, 0xf9, 0x2e, 0x0f, 0x18,
    0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04,
    0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20,
    0xf7, 0x1d, 0x20, 0xf2, 0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62,
    0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06, 0x7b, 0xe2, 0x0c, 0x3e,
    0x87, 0xf2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20,
    0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17,
    0xc1, 0xcb, 0x11, 0x17, 0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9,
    0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83,
    0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
    0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63,
    0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e,
    0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x4c, 0x21, 0x04, 0x01, 0x11,
    0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
    0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe,
    0x3e, 0x01, 0xe0, 0x50};

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
        [ROM_PLAIN] = "ROM_PLAIN",
        [ROM_MBC1] = "ROM_MBC1",
        [ROM_MBC1_RAM] = "ROM_MBC1",
        [ROM_MBC1_RAM_BATT] = "ROM_MBC1_RAM_BATT",
        [ROM_MBC2] = "ROM_MBC2",
        [ROM_MBC2_BATTERY] = "ROM_MBC2_BATTERY",
        [ROM_RAM] = "ROM_RAM",
        [ROM_RAM_BATTERY] = "ROM_RAM_BATTERY",
        [ROM_MMM01] = "ROM_MMM01",
        [ROM_MMM01_SRAM] = "ROM_MMM01_SRAM",
        [ROM_MMM01_SRAM_BATT] = "ROM_MMM01_SRAM_BATT",
        [ROM_MBC3_TIMER_BATT] = "ROM_MBC3_TIMER_BATT",
        [ROM_MBC3_TIMER_RAM_BATT] = "ROM_MBC3_TIMER_RAM_BATT",
        [ROM_MBC3] = "ROM_MBC3",
        [ROM_MBC3_RAM] = "ROM_MBC3_RAM",
        [ROM_MBC3_RAM_BATT] = "ROM_MBC3_RAM_BATT",
        [ROM_MBC5] = "ROM_MBC5",
        [ROM_MBC5_RAM] = "ROM_MBC5_RAM",
        [ROM_MBC5_RAM_BATT] = "ROM_MBC5_RAM_BATT",
        [ROM_MBC5_RUMBLE] = "ROM_MBC5_RUMBLE",
        [ROM_MBC5_RUMBLE_SRAM] = "ROM_MBC5_RUMBLE_SRAM",
        [ROM_MBC5_RUMBLE_SRAM_BATT] = "ROM_MBC5_RUMBLE_SRAM_BATT",
        [ROM_POCKET_CAMERA] = "ROM_POCKET_CAMERA",
        [ROM_BANDAI_TAMA5] = "ROM_BANDAI_TAMA5",
        [ROM_HUDSON_HUC3] = "ROM_HUDSON_HUC3",
        [ROM_HUDSON_HUC1] = "ROM_HUDSON_HUC1",
};

static int cart_load_header(void)
{
    if (CART.rom_size < sizeof(cart_header_t)) {
        fprintf(stderr, "ERROR: rom too small!\n");
        return -1;
    }
    /* Copy header pointer. */
    CART.header = (cart_header_t *)&CART.rom[ROM_OFFSET_TITLE];
    printf("Game title: %s\n", CART.header->title);
    /* Get cart type. */
    printf("Cartridge type: %s\n", g_rom_types[CART.header->cart_type]);
    if (CART.header->cart_type != ROM_PLAIN) {
        fprintf(stderr, "Only 32KB games with no mappers are supported!\n");
        return -1;
    }
    /* Get ROM size. */
    unsigned int rom_size_tmp = (unsigned int)powf(2.0, (float)(5 + (0xf & CART.header->rom_size)));
    printf("ROM size: %hhu = %uKB\n", CART.header->rom_size, rom_size_tmp);
    if (CART.header->rom_size != 0) {
        fprintf(stderr, "Only 32KB games with no mappers are supported!\n");
        return -1;
    }
    if (CART.rom_size != rom_size_tmp * 1024) {
        fprintf(stderr, "ROM file size does not equal header ROM size!\n");
        return -1;
    }
    /* Get RAM size. */
    unsigned int ram_size_tmp = CART.header->ram_size;
    if (ram_size_tmp > 0) {
        ram_size_tmp = (unsigned int)powf(4.0, (float)(ram_size_tmp)) / 2;
    }
    printf("RAM size: %hhu = %uKB\n", CART.header->ram_size, ram_size_tmp);
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
    /* Create cart. */
    CART.rom_size = size;
    CART.rom = malloc(size);
    /* Read rom to memory. */
    size_t read_size = fread(CART.rom, 1, size, file);
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
    return 0;
}

void cart_unload(void)
{
    free(CART.rom);
}

uint8_t cart_read_rom(uint16_t addr)
{
    return CART.rom[addr];
}

uint8_t read_internal_rom(uint16_t addr)
{
    return g_internal_rom[addr];
}
