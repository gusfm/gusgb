#ifndef __CART_H__
#define __CART_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

typedef enum {
    CART_ROM_ONLY = 0x00,
    CART_MBC1 = 0x01,
    CART_MBC1_RAM = 0x02,
    CART_MBC1_RAM_BATTERY = 0x03,
    CART_MBC2 = 0x05,
    CART_MBC2_BATTERY = 0x06,
    CART_ROM_RAM = 0x08,
    CART_ROM_RAM_BATTERY = 0x09,
    CART_MMM01 = 0x0B,
    CART_MMM01_RAM = 0x0C,
    CART_MMM01_RAM_BATTERY = 0x0D,
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
} cart_type_e;

typedef struct {
    uint8_t title[15];    /* 0x134-0x142: Title */
    uint8_t cgb;          /* 0x143: $80 = GBC, other = not GBC */
    uint8_t lcode_new_h;  /* 0x144: licensee code (new) high */
    uint8_t lcode_new_l;  /* 0x145: licensee code (new) low */
    uint8_t sgb;          /* 0x146: GB/SGB Indicator */
    uint8_t cart_type;    /* 0x147: Cartridge type */
    uint8_t rom_size;     /* 0x148: ROM size */
    uint8_t ram_size;     /* 0x149: RAM size */
    uint8_t country_code; /* 0x14a: Country destination Code */
    uint8_t licensee_old; /* 0x14b: licensee code (old) */
    uint8_t mask_rom;     /* 0x14c: Mask ROM Version number */
    uint8_t compl_check;  /* 0x14d: Complement check */
    uint8_t checksum_h;   /* 0x14e: checksum high */
    uint8_t checksum_l;   /* 0x14f: checksum low */
} cart_header_t;

typedef struct {
    uint8_t *bytes;
    size_t size;
    cart_header_t *header;
    unsigned int offset;
} cart_rom_t;

typedef struct {
    uint8_t *bytes;
    size_t size;
    unsigned int offset;
    char *path;
} cart_ram_t;

typedef void (*mbc_write_f)(uint16_t addr, uint8_t val);

typedef struct {
    uint8_t rom_bank;
    uint8_t ram_bank;
    bool ram_on;
    uint8_t mode;
    mbc_write_f write_func;
} cart_mbc_t;

typedef struct {
    cart_type_e type;
    cart_rom_t rom;
    cart_ram_t ram;
    cart_mbc_t mbc;
} cart_t;

int cart_load(const char *path);
void cart_unload(void);
uint8_t cart_read_rom0(uint16_t addr);
uint8_t cart_read_rom1(uint16_t addr);
void cart_write_mbc(uint16_t addr, uint8_t val);
uint8_t cart_read_ram(uint16_t addr);
void cart_write_ram(uint16_t addr, uint8_t val);
bool cart_is_cgb(void);

#endif /* __CART_H__ */
