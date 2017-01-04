#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

typedef struct {
    uint32_t main; /* Internal timer clock. 1/4 of cpu main clock. */
    uint32_t sub;  /* Variable to help internal clock count. */
    uint32_t div;  /* Variable to help div timer count. */
} clock_t;

typedef struct {
    uint8_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;
    clock_t clock;
} timer_t;

void timer_init(void);
void timer_step(uint32_t clock_step);
uint8_t timer_read_byte(uint16_t addr);
void timer_write_byte(uint16_t addr, uint8_t val);

#endif /* __TIMER_H__ */
