#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

typedef struct {
    uint32_t main;
    uint32_t sub;
    uint32_t div;
} clock_t;

typedef struct {
    uint8_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;
    clock_t clock;
} timer_t;

void timer_init(void);
void timer_step(uint32_t cpu_tick);
uint8_t timer_read_byte(uint16_t addr);
void timer_write_byte(uint16_t addr, uint8_t val);

#endif /* __TIMER_H__ */
