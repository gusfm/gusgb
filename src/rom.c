#include "rom.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint8_t g_internal_rom[0x100] = {
    0x0F, 0x00, 0x7C, 0xFF, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x01, 0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00,
    0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF, 0xFF, 0x00, 0x00, 0xBF,
    0x77, 0xF3, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
    0x00, 0xFF, 0x00, 0xFF, 0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xC1, 0x00,
    0xFE, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCE, 0xED, 0x66, 0x66,
    0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6,
    0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x45, 0xEC, 0x52, 0xFA,
    0x08, 0xB7, 0x07, 0x5D, 0x01, 0xFD, 0xC0, 0xFF, 0x08, 0xFC, 0x00, 0xE5,
    0x0B, 0xF8, 0xC2, 0xCE, 0xF4, 0xF9, 0x0F, 0x7F, 0x45, 0x6D, 0x3D, 0xFE,
    0x46, 0x97, 0x33, 0x5E, 0x08, 0xEF, 0xF1, 0xFF, 0x86, 0x83, 0x24, 0x74,
    0x12, 0xFC, 0x00, 0x9F, 0xB4, 0xB7, 0x06, 0xD5, 0xD0, 0x7A, 0x00, 0x9E,
    0x04, 0x5F, 0x41, 0x2F, 0x1D, 0x77, 0x36, 0x75, 0x81, 0xAA, 0x70, 0x3A,
    0x98, 0xD1, 0x71, 0x02, 0x4D, 0x01, 0xC1, 0xFF, 0x0D, 0x00, 0xD3, 0x05,
    0xF9, 0x00, 0x0B, 0x00};

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
        char c = rom_header[i + ROM_OFFSET_NAME];
        if (c == 0x80 || c == 0xc0)
            rom_name[i] = '\0';
        else
            rom_name[i] = c;
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
    int rom_size = rom_header[ROM_OFFSET_ROM_SIZE];
    if ((rom_size & 0xF0) == 0x50)
        rom_size = (int)pow(2.0, (double)(((0x52) & 0xF) + 1)) + 64;
    else
        rom_size = (int)pow(2.0, (double)(rom_size + 1));
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
    long size = ftell(file);
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
