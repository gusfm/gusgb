#ifndef APU_H
#define APU_H

#include <stdint.h>

void apu_write_byte(uint16_t addr, uint8_t value);
uint8_t apu_read_byte(uint16_t addr);

#endif /* APU_H */
