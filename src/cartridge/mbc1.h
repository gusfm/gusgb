#ifndef __MBC1_H__
#define __MBC1_H__

#include <stdint.h>

void mbc1_init(void);
void mbc1_write(uint16_t addr, uint8_t val);
uint8_t mbc1_ram_read(uint16_t addr);
void mbc1_ram_write(uint16_t addr, uint8_t val);

#endif /* __MBC1_H__ */
