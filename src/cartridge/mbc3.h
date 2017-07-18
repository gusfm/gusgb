#ifndef __MBC3_H__
#define __MBC3_H__

#include <stdint.h>

void mbc3_init(void);
void mbc3_write(uint16_t addr, uint8_t val);
uint8_t mbc3_ram_read(uint16_t addr);
void mbc3_ram_write(uint16_t addr, uint8_t val);

#endif /* __MBC3_H__ */
