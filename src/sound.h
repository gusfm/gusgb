#ifndef __SOUND_H__
#define __SOUND_H__

#include <stdint.h>

void sound_write_byte(uint16_t addr, uint8_t value);
uint8_t sound_read_byte(uint16_t addr);

#endif /* __SOUND_H__ */
