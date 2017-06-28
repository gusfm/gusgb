#ifndef __MMU_H__
#define __MMU_H__

#include <stdbool.h>
#include <stdint.h>

typedef void (*switch_ext_rom_cb_t)(void);

typedef struct {
    uint8_t wram[8][0x1000]; /* Working RAM. */
    uint8_t zram[0x80];   /* Zero-page RAM. */
    uint8_t io[0x100];    /* Memory-mapped I/O. */
    uint8_t read_ext_rom;
    uint8_t cart_type;
    /* FF4D - KEY1 - CGB Mode Only - Prepare Speed Switch */
    uint8_t speed_switch;
    /* FF70 - SVBK - CGB Mode Only - WRAM Bank */
    unsigned int wram_bank;
    switch_ext_rom_cb_t switch_ext_rom_cb;
} mmu_t;

/* Init MMU subsystem. */
int mmu_init(const char *rom_path, switch_ext_rom_cb_t cb);

/* Free cartridge memory. */
void mmu_finish(void);

/* Reset MMU keeping cartridge ROM memory. */
void mmu_reset(void);

/* Read byte from a given address. */
uint8_t mmu_read_byte(uint16_t addr);

/* Read word from a given address. */
uint16_t mmu_read_word(uint16_t addr);

/* Write byte to a given address. */
void mmu_write_byte(uint16_t addr, uint8_t value);

/* Write word to a given address. */
void mmu_write_word(uint16_t addr, uint16_t value);

/* Stop MMU, and change speed if requested. */
void mmu_stop(void);

/* Debug MMU. */
void mmu_dump(uint16_t addr, uint16_t offset);

#endif /* __MMU_H__ */
