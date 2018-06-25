#ifndef MBC5_H
#define MBC5_H

#include <stdint.h>

void mbc5_init(void);
void mbc5_write(uint16_t addr, uint8_t val);
uint8_t mbc5_ram_read(uint16_t addr);
void mbc5_ram_write(uint16_t addr, uint8_t val);

#endif /* MBC5_H */
