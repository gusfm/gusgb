#include "rom.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static int rom_load_header(FILE *file, size_t file_size)
{
    if (file_size < ROM_HEADER_SIZE) {
        fprintf(stderr, "ERROR: rom too small!\n");
        return -1;
    }
    /* Read ROM header. */
    uint8_t rom_header[ROM_HEADER_SIZE];
    size_t read_size = fread(rom_header, 1, ROM_HEADER_SIZE, file);
    if (read_size != ROM_HEADER_SIZE) {
        fclose(file);
        fprintf(stderr, "ERROR: fread rom header\n");
        return -1;
    }
    /* Get ROM name. */
    char rom_name[17];
    memset(rom_name, '\0', 17);
    for (int i = 0; i < 16; i++) {
        uint8_t c = rom_header[i + ROM_OFFSET_NAME];
        if (c == 0x80 || c == 0xc0)
            rom_name[i] = '\0';
        else
            rom_name[i] = (char)c;
    }
    printf("Internal ROM name: %s\n", rom_name);
    /* Get ROM type. */
    enum rom_type_e rom_type;
    rom_type = rom_header[ROM_OFFSET_TYPE];
    if (!g_rom_types[rom_type]) {
        fprintf(stderr, "Unknown ROM type: %#02x\n", rom_type);
        return -1;
    }
    printf("ROM type: %s\n", g_rom_types[rom_type]);
    if (rom_type != ROM_PLAIN) {
        fprintf(stderr, "Only 32KB games with no mappers are supported!\n");
        return -1;
    }
    /* Get ROM size. */
    unsigned int rom_size = rom_header[ROM_OFFSET_ROM_SIZE];
    if ((rom_size & 0xF0) == 0x50)
        rom_size = (unsigned int)pow(2.0, (double)(((0x52) & 0xF) + 1)) + 64;
    else
        rom_size = (unsigned int)pow(2.0, (double)(rom_size + 1));
    printf("ROM size: %dKB\n", rom_size * 16);
    if (rom_size * 16 != 32) {
        fprintf(stderr, "Only 32KB games with no mappers are supported!\n");
        return -1;
    }
    if (file_size != rom_size * 16 * 1024) {
        fprintf(stderr, "ROM filesize does not equal ROM size!\n");
        return -1;
    }
    /* Get RAM size. */
    int ram_size = rom_header[ROM_OFFSET_RAM_SIZE];
    ram_size = (int)pow(4.0, (double)(ram_size)) / 2;
    printf("RAM size: %dKB\n", ram_size);
    rewind(file);
    return 0;
}

int rom_load(const char *path, uint8_t *buffer, size_t bufsize)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }
    /* Get file size. */
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    if (size > bufsize) {
        fclose(file);
        fprintf(stderr, "ERROR: rom too big!\n");
        return -1;
    }
    rewind(file);
    /* Read ROM header. */
    int ret = rom_load_header(file, size);
    if (ret < 0) {
        fclose(file);
        fprintf(stderr, "ERROR: rom_load_header\n");
        return -1;
    }
    /* Read rom to memory. */
    size_t read_size = fread(buffer, 1, size, file);
    fclose(file);
    if (read_size != size) {
        fprintf(stderr, "ERROR: fread\n");
        return -1;
    }
    return 0;
}

uint8_t rom_read_internal(uint16_t addr)
{
    return g_internal_rom[addr];
}
