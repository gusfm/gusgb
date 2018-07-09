#include "cart.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "mbc1.h"
#include "mbc3.h"
#include "mbc5.h"

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

static int cart_get_mbc(uint8_t cart_type, cart_mbc_t *mbc)
{
    switch (cart_type) {
        case CART_ROM_ONLY:
        case CART_MBC1:
        case CART_MBC1_RAM:
        case CART_MBC1_RAM_BATTERY:
            mbc->init = mbc1_init;
            mbc->write = mbc1_write;
            mbc->ram_read = mbc1_ram_read;
            mbc->ram_write = mbc1_ram_write;
            return 0;
        case CART_MBC3_TIMER_BATTERY:
        case CART_MBC3_TIMER_RAM_BATTERY:
        case CART_MBC3:
        case CART_MBC3_RAM:
        case CART_MBC3_RAM_BATTERY:
            mbc->init = mbc3_init;
            mbc->write = mbc3_write;
            mbc->ram_read = mbc3_ram_read;
            mbc->ram_write = mbc3_ram_write;
            return 0;
        case CART_MBC5:
        case CART_MBC5_RAM:
        case CART_MBC5_RAM_BATTERY:
        case CART_MBC5_RUMBLE:
        case CART_MBC5_RUMBLE_RAM:
        case CART_MBC5_RUMBLE_RAM_BATTERY:
            mbc->init = mbc5_init;
            mbc->write = mbc5_write;
            mbc->ram_read = mbc5_ram_read;
            mbc->ram_write = mbc5_ram_write;
            return 0;
        default:
            return -1;
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

static bool cart_has_rtc(uint8_t cart_type)
{
    switch (cart_type) {
        case CART_MBC3_TIMER_BATTERY:
        case CART_MBC3_TIMER_RAM_BATTERY:
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
    printf("CGB: 0x%.2x (%s)\n", CART.rom.header->cgb, cart_is_cgb() ? "true" : "false");
    /* Get cart type. */
    CART.type = header->cart_type;
    printf("Cartridge type: %s\n", g_rom_types[CART.type]);
    int ret = cart_get_mbc(CART.type, &CART.mbc);
    if (ret != 0) {
        fprintf(stderr, "Cartridge type not supported!\n");
        return -1;
    }
    /* Get ROM size. */
    unsigned int rom_size_tmp = 0x8000 << header->rom_size;
    printf("ROM size: %hhu = %uKB\n", header->rom_size, rom_size_tmp / 1024);
    if (CART.rom.size != rom_size_tmp) {
        fprintf(stderr, "ROM file size does not equal header ROM size!\n");
        return -1;
    }
    CART.rom.max_bank = (1 << header->rom_size) * 2;
    /* Get RAM size. */
    switch (header->ram_size) {
        case 0:
            CART.ram.size = 0;
            CART.ram.max_bank = 1;
            break;
        case 1:
            CART.ram.size = 2 * 1024;
            CART.ram.max_bank = 1;
            break;
        case 2:
            CART.ram.size = 8 * 1024;
            CART.ram.max_bank = 1;
            break;
        case 3:
            CART.ram.size = 32 * 1024;
            CART.ram.max_bank = 4;
            break;
        case 4:
            CART.ram.size = 128 * 1024;
            CART.ram.max_bank = 16;
            break;
        case 5:
            CART.ram.size = 64 * 1024;
            CART.ram.max_bank = 8;
            break;
    }
    printf("RAM size: %hhu = %luKB\n", header->ram_size, CART.ram.size / 1024);
    return 0;
}

static char *card_get_ram_path(const char *rom_path)
{
    size_t rom_path_len = strlen(rom_path);
    size_t ram_path_len = rom_path_len + 2;
    char *ram_path = malloc(ram_path_len);
    int ret = snprintf(ram_path, ram_path_len, "%s", rom_path);
    if (ret != (int)rom_path_len) {
        return NULL;
    }
    char *substr = strstr(ram_path, ".");
    if (substr == NULL) {
        return NULL;
    }
    ret = snprintf(substr, 5, ".sav");
    if (ret != 4) {
        return NULL;
    }
    return ram_path;
}

static int cart_ram_init(FILE *ram_save_file)
{
    CART.ram.bytes = malloc(CART.ram.size);
    CART.ram.offset = 0x0000;
    if (ram_save_file) {
        printf("Loading cartridge RAM from file: %s\n", CART.ram.path);
        size_t rv;
        rv = fread(CART.ram.bytes, 1, CART.ram.size, ram_save_file);
        if (rv != CART.ram.size) {
            perror("fread ram:");
            return -1;
        }
    } else {
        memset(CART.ram.bytes, 0, CART.ram.size);
    }
    CART.ram.enabled = false;
    /* Init RTC if present. */
    if (cart_has_rtc(CART.type)) {
        if (mbc3_rtc_load(ram_save_file) < 0) {
            return -1;
        }
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
            fprintf(stderr, "ERROR: Could not save cartridge RAM to %s\n",
                    CART.ram.path);
            fclose(f);
            return;
        }
        printf("Cartridge RAM saved to file: %s\n", CART.ram.path);
        if (cart_has_rtc(CART.type)) {
            mbc3_rtc_save(f);
        }
        fclose(f);
    }
}

static void cart_destroy(void)
{
    free(CART.ram.path);
    free(CART.rom.bytes);
    free(CART.ram.bytes);
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
        cart_destroy();
        return -1;
    }
    CART.ram.path = card_get_ram_path(path);
    FILE *ram_save_file = fopen(CART.ram.path, "r");
    /* Init RAM. */
    ret = cart_ram_init(ram_save_file);
    if (ret < 0) {
        if (ram_save_file)
            fclose(ram_save_file);
        cart_destroy();
        return -1;
    }
    if (ram_save_file)
        fclose(ram_save_file);
    /* Init MBC. */
    CART.mbc.init();
    return 0;
}

void cart_unload(void)
{
    cart_ram_save();
    cart_destroy();
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
    CART.mbc.write(addr, val);
}

uint8_t cart_read_ram(uint16_t addr)
{
    return CART.mbc.ram_read(addr);
}

void cart_write_ram(uint16_t addr, uint8_t val)
{
    CART.mbc.ram_write(addr, val);
}

bool cart_is_cgb(void)
{
    return CART.rom.header->cgb & 0x80;
}
