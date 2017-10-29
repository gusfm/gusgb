#ifndef CPU_UTILS_H
#define CPU_UTILS_H

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

#endif /* CPU_UTILS_H */
