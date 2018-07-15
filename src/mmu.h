#ifndef MMU_H
#define MMU_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*switch_ext_rom_cb_t)(void);

typedef struct {
    uint8_t wram[8][0x1000]; /* Working RAM. */
    uint8_t zram[0x80];      /* Zero-page RAM. */
    uint8_t speed_switch;    /* 0xff4d (KEY1): Prepare Speed Switch */
    uint8_t hdma1;           /* 0xff51 (HDMA1): DMA data src high */
    uint8_t hdma2;           /* 0xff52 (HDMA2): DMA data src low */
    uint8_t hdma3;           /* 0xff53 (HDMA3): DMA data dest high */
    uint8_t hdma4;           /* 0xff54 (HDMA4): DMA data dest low */
    uint8_t hdma5;           /* 0xff55 (HDMA5): DMA mode */
    uint8_t ir;              /* 0xff56 (RP): Infrared Port */
    uint8_t wram_bank;       /* 0xff70 (SVBK): WRAM Bank */
    uint8_t clock_speed;     /* 0: normal speed; 1: double speed */
} mmu_t;

/* Init MMU subsystem. */
int mmu_init(const char *rom_path);

/* Free cartridge memory. */
void mmu_finish(void);

/* Reset MMU keeping cartridge ROM memory. */
void mmu_reset(void);

/* Read byte from a given address withoud accounting for time delay. */
uint8_t mmu_read_byte_dma(uint16_t addr);

/* Read byte from a given address. */
uint8_t mmu_read_byte(uint16_t addr);

/* Read word from a given address. */
uint16_t mmu_read_word(uint16_t addr);

/* Write byte to a given address without accounting for time delay. */
void mmu_write_byte_dma(uint16_t addr, uint8_t value);

/* Write byte to a given address. */
void mmu_write_byte(uint16_t addr, uint8_t value);

/* Write word to a given address. */
void mmu_write_word(uint16_t addr, uint16_t value);

/* Stop MMU, and change speed if requested. */
void mmu_stop(void);

/* Debug MMU. */
void mmu_dump(uint16_t addr, uint16_t offset);

#endif /* MMU_H */
