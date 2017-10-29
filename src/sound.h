#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

void sound_write_byte(uint16_t addr, uint8_t value);
uint8_t sound_read_byte(uint16_t addr);

#endif /* SOUND_H */
