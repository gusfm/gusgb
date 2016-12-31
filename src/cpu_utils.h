#ifndef __CPU_UTILS_H__
#define __CPU_UTILS_H__

#include <stdint.h>

uint8_t rlc(uint8_t value);
uint8_t rrc(uint8_t value);
uint8_t rl(uint8_t value);
uint8_t rr(uint8_t value);
uint8_t sla(uint8_t value);
uint8_t sra(uint8_t value);
uint8_t swap(uint8_t value);
uint8_t srl(uint8_t value);
void bit(uint8_t bit, uint8_t value);
uint8_t res(uint8_t bit, uint8_t value);
uint8_t set(uint8_t bit, uint8_t value);

#endif /* __CPU_UTILS_H__ */