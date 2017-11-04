#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_init(void);
void timer_step(uint32_t clock_step);
uint8_t timer_read_byte(uint16_t addr);
void timer_write_byte(uint16_t addr, uint8_t val);
void timer_change_speed(unsigned int speed);
void timer_dump(void);

#endif /* TIMER_H */
