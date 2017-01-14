#ifndef __ROM_H__
#define __ROM_H__

#include <stddef.h>
#include <stdint.h>

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
    uint8_t *rom;
    size_t rom_size;
    cart_header_t *header;
} cart_t;

int cart_load(const char *path);
void cart_unload(void);
uint8_t cart_read_rom(uint16_t addr);
uint8_t read_internal_rom(uint16_t addr);

#endif /* __ROM_H__ */
