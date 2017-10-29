#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct {
    uint32_t main; /* Clock used for timer count. */
    uint16_t internal; /* Internal timer clock. */
    unsigned int select;
    unsigned int speed;
} gb_clock_t;

typedef struct {
    uint8_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;
    gb_clock_t clock;
} gb_timer_t;

void timer_init(void);
void timer_step(uint32_t clock_step);
uint8_t timer_read_byte(uint16_t addr);
void timer_write_byte(uint16_t addr, uint8_t val);
void timer_change_speed(unsigned int speed);

#endif /* TIMER_H */
