#include "cart.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "mbc1.h"

#define ROM_OFFSET_TITLE 0x134

cart_t CART;

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
    [CART_MMM01_RAM] = "MMM01+RAM",
    [CART_MMM01_RAM_BATTERY] = "MMM01+RAM+BATTERY",
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

static bool cart_has_battery(uint8_t cart_type)
{
    switch (cart_type) {
        case CART_MBC1_RAM_BATTERY:
        case CART_ROM_RAM_BATTERY:
        case CART_MMM01_RAM_BATTERY:
        case CART_MBC3_TIMER_RAM_BATTERY:
        case CART_MBC3_RAM_BATTERY:
        case CART_MBC4_RAM_BATTERY:
        case CART_MBC5_RAM_BATTERY:
        case CART_MBC5_RUMBLE_RAM_BATTERY:
        case CART_HUC1_RAM_BATTERY:
            return true;
        default:
            return false;
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
    CART.type = header->cart_type;
    printf("Cartridge type: %s\n", g_rom_types[CART.type]);
    CART.mbc.write_func = cart_get_mbc(CART.type);
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
    CART.ram.size = ram_size_tmp * 1024;
    printf("RAM size: %hhu = %uKB\n", header->ram_size, ram_size_tmp);
    return 0;
}

static int cart_ram_init(const char *rom_path)
{
    size_t rom_path_len = strlen(rom_path);
    size_t ram_path_len = rom_path_len + 2;
    CART.ram.path = malloc(ram_path_len);
    int ret = snprintf(CART.ram.path, ram_path_len, "%s", rom_path);
    if (ret != (int)rom_path_len) {
        return -1;
    }
    char *substr = strstr(CART.ram.path, ".");
    if (substr == NULL) {
        return -1;
    }
    ret = snprintf(substr, 5, ".sav");
    if (ret != 4) {
        return -1;
    }
    CART.ram.bytes = malloc(CART.ram.size);
    CART.ram.offset = 0x0000;
    FILE *f = fopen(CART.ram.path, "r");
    if (f) {
        printf("Loading cartridge RAM from file: %s\n", CART.ram.path);
        size_t rv;
        rv = fread(CART.ram.bytes, 1, CART.ram.size, f);
        if (rv != CART.ram.size) {
            perror("fread:");
            return -1;
        }
        fclose(f);
    } else {
        memset(CART.ram.bytes, 0, CART.ram.size);
    }
    return 0;
}

static void cart_ram_save(void)
{
    if (cart_has_battery(CART.type) && CART.ram.path != NULL) {
        FILE *f = fopen(CART.ram.path, "w");
        if (f == NULL) {
            fprintf(stderr, "ERROR: Could not open %s\n", CART.ram.path);
            return;
        }
        size_t rv = fwrite(CART.ram.bytes, 1, CART.ram.size, f);
        if (rv != CART.ram.size) {
            fprintf(stderr, "ERROR: Could save cartridge RAM to %s\n", CART.ram.path);
        }
        fclose(f);
    }
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
    ret = cart_ram_init(path);
    if (ret < 0) {
        cart_unload();
        return -1;
    }
    /* Init MBC. */
    CART.mbc.rom_bank = 0;
    CART.mbc.ram_bank = 0;
    CART.mbc.ram_on = false;
    CART.mbc.mode = 0;
    return 0;
}

void cart_unload(void)
{
    cart_ram_save();
    free(CART.ram.path);
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
