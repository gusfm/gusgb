#include "timer.h"

#include <stdio.h>

#include "cartridge/cart.h"
#include "debug.h"
#include "interrupt.h"

#define TIMER_ENABLE (1 << 2)

/* TIMA states. */
typedef enum {
    TIMA_STATE_COUNTING = 0, /* Normal operation */
    TIMA_STATE_OVERFLOW,     /* Overflow happened. */
    TIMA_STATE_RELOADING,    /* TIMA is being reloaded. */
} tima_state_t;

static unsigned int clk_sys;    /* Internal timer clock. */
static uint8_t tima;            /* [$ff05] Timer counter (R/W) */
static uint8_t tma;             /* [$ff06] Timer Modulo (R/W) */
static uint8_t tac;             /* [$ff07] Timer Control (R/W) */
static tima_state_t tima_state; /* TIMA operation states. */
static unsigned int delay_bit;  /* Falling edge detector delay bit. */
static const unsigned int masks[4] = {0x200, 0x8, 0x20, 0x80};
static unsigned int timer_enabled;
static unsigned int timer_mask;

void timer_reset(void)
{
    tima = 0;
    tma = 0;
    tac = 0;
    timer_enabled = 0;
    timer_mask = masks[0];
    if (cart_is_cgb())
        clk_sys = 0x2674; /* Initial value for CGB ABCDE */
    else
        clk_sys = 0xabca; /* Initial value for DMG ABC */
    tima_state = TIMA_STATE_COUNTING;
    delay_bit = 0;
}

void timer_step(unsigned int clock_step)
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
    unsigned int bit = (clk_sys & timer_mask) && timer_enabled;
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
    return 0xf8 | tac;
}

void timer_write_tac(uint8_t val)
{
    tac = 7 & val;
    timer_enabled = tac >> 2;
    timer_mask = masks[tac & 3];
}

void timer_dump(void)
{
    printf("Timer dump:\n");
    printf("[$ff04] div=0x%.2x\n", timer_read_div());
    printf("[$ff05] tima=0x%.2x\n", timer_read_tima());
    printf("[$ff06] tma=0x%.2x\n", timer_read_tma());
    printf("[$ff07] tac=0x%.2x\n", timer_read_tac());
}
