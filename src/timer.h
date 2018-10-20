#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_reset(void);
void timer_step(unsigned int clock_step);

uint8_t timer_read_div(void);
uint8_t timer_read_tima(void);
uint8_t timer_read_tma(void);
uint8_t timer_read_tac(void);

void timer_write_div(void);
void timer_write_tima(uint8_t val);
void timer_write_tma(uint8_t val);
void timer_write_tac(uint8_t val);

void timer_write_byte(uint16_t addr, uint8_t val);
void timer_dump(void);

#endif /* TIMER_H */
