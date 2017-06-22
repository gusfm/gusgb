#include "timer.h"
#include <stdio.h>
#include "interrupt.h"

gb_timer_t TIMER;

#define TIMER_ENABLE (1 << 2)
#define TIMER_SPEED (0x3)

void timer_init(void)
{
    TIMER.div = 0;
    TIMER.tima = 0;
    TIMER.tma = 0;
    TIMER.tac = 0;
    TIMER.clock.main = 0;
    TIMER.clock.sub = 0;
    TIMER.clock.div = 0;
}

static void timer_check(void)
{
    /* Check whether a step needs to be made in the timer. */
    if (TIMER.tac & TIMER_ENABLE) {
        uint32_t threshold;
        switch (TIMER.tac & TIMER_SPEED) {
            case 0:
                /* 4096 Hz. */
                threshold = 64;
                break;
            case 1:
                /* 262144 Hz. */
                threshold = 1;
                break;
            case 2:
                /* 65536 Hz. */
                threshold = 4;
                break;
            case 3:
                /* 16384 Hz. */
                threshold = 16;
                break;
        }
        if (TIMER.clock.main >= threshold) {
            TIMER.clock.main = 0;
            if (TIMER.tima == 0xff) {
                /* If TIMA overflows, load it with TMA and generate interrupt.
                 */
                TIMER.tima = TIMER.tma;
                interrupt_set_flag_bit(INTERRUPTS_TIMER);
            } else {
                TIMER.tima++;
            }
        }
    }
}

void timer_step(uint32_t clock_step)
{
    TIMER.clock.sub += clock_step;
    /* Internal timer clock counts at 1/4 of main clock. */
    if (TIMER.clock.sub >= 16) {
        TIMER.clock.sub -= 16;
        TIMER.clock.main++;
        TIMER.clock.div++;
        /* DIV runs at 1/16 of timer main clock. */
        if (TIMER.clock.div == 64) {
            TIMER.clock.div = 0;
            TIMER.div++;
        }
    }
    timer_check();
}

uint8_t timer_read_byte(uint16_t addr)
{
    switch (addr) {
        case 0xFF04:
            return TIMER.div;
        case 0xFF05:
            return TIMER.tima;
        case 0xFF06:
            return TIMER.tma;
        case 0xFF07:
            return TIMER.tac;
        default:
            printf("%s: not implemented: 0x%04x!\n", __func__, addr);
            return 0;
    }
}

void timer_write_byte(uint16_t addr, uint8_t val)
{
    switch (addr) {
        case 0xFF04:
            TIMER.div = 0;
            break;
        case 0xFF05:
            TIMER.tima = val;
            break;
        case 0xFF06:
            TIMER.tma = val;
            break;
        case 0xFF07:
            TIMER.tac = val & 7;
            break;
        default:
            printf("%s: not implemented: 0x%04x!\n", __func__, addr);
            break;
    }
}
