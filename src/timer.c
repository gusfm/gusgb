#include "timer.h"
#include <stdio.h>
#include "debug.h"
#include "interrupt.h"

#define TIMER_ENABLE (1 << 2)

/* TIMA states. */
typedef enum {
    TIMA_STATE_COUNTING = 0, /* Normal operation */
    TIMA_STATE_OVERFLOW,     /* Overflow happened. */
    TIMA_STATE_RELOADING,    /* TIMA is being reloaded. */
} tima_state_t;

unsigned int clk_speed;  /* Clock speed: 0 or 1. */
uint16_t clk_sys;        /* Internal timer clock. */
uint8_t tima;            /* [$ff05] Timer counter (R/W) */
uint8_t tma;             /* [$ff06] Timer Modulo (R/W) */
uint8_t tac;             /* [$ff07] Timer Control (R/W) */
tima_state_t tima_state; /* TIMA operation states. */
unsigned int delay_bit;  /* Falling edge detector delay bit. */
static uint16_t masks[4] = {0x200, 0x8, 0x20, 0x80};

void timer_reset(void)
{
    clk_speed = 0;
    tima = 0;
    tma = 0;
    tac = 0;
    clk_sys = 0;
    tima_state = TIMA_STATE_COUNTING;
    delay_bit = 0;
}

static inline unsigned int mux(uint8_t sel, uint16_t in)
{
    return in & (masks[sel & 3] << clk_speed);
}

void timer_step(uint32_t clock_step)
{
    clk_sys += clock_step;
    /* Check whether a step needs to be made in the timer. */
    switch (tima_state) {
        case TIMA_STATE_COUNTING:
            break;
        case TIMA_STATE_OVERFLOW:
            /* After one cycle, TIMA should be reloaded and the interrupt
             * flag set.
             */
            tima = tma;
            interrupt_raise(INTERRUPTS_TIMER);
            tima_state = TIMA_STATE_RELOADING;
            break;
        case TIMA_STATE_RELOADING:
            tima_state = TIMA_STATE_COUNTING;
            break;
    }
    /* Falling edge detector. */
    unsigned int bit = mux(tac, clk_sys) && (tac & TIMER_ENABLE);
    if (delay_bit & ~bit) {
        if (++tima == 0) {
            /* When TIMA overflows, it contains zero for 1 cycle. */
            tima_state = TIMA_STATE_OVERFLOW;
        }
    }
    delay_bit = bit;
}

uint8_t timer_read_div(void)
{
    return clk_sys >> 8;
}

void timer_write_div(void)
{
    clk_sys = 0;
}

uint8_t timer_read_tima(void)
{
    return tima;
}

void timer_write_tima(uint8_t val)
{
    switch (tima_state) {
        case TIMA_STATE_COUNTING:
            /* Normal operation. */
            tima = val;
            break;
        case TIMA_STATE_OVERFLOW:
            /* Prevent reload if writing to it during overflow cycle. */
            tima = val;
            tima_state = TIMA_STATE_COUNTING;
            break;
        case TIMA_STATE_RELOADING:
            /* Ignore writes if TIMA was just reloaded. */
            break;
    }
}

uint8_t timer_read_tma(void)
{
    return tma;
}

void timer_write_tma(uint8_t val)
{
    tma = val;
    if (tima_state == TIMA_STATE_RELOADING) {
        /* If TMA is written in the same cycle that TIMA is reloaded,
         * write the value to TIMA as well. */
        tima = tma;
    }
}

uint8_t timer_read_tac(void)
{
    return tac;
}

void timer_write_tac(uint8_t val)
{
    tac = val;
}

void timer_change_speed(unsigned int speed)
{
    clk_speed = speed;
}

void timer_dump(void)
{
    printf("Timer dump:\n");
    printf("[$ff04] div=0x%.2x\n", timer_read_div());
    printf("[$ff05] tima=0x%.2x\n", timer_read_tima());
    printf("[$ff06] tma=0x%.2x\n", timer_read_tma());
    printf("[$ff07] tac=0x%.2x\n", timer_read_tac());
}
