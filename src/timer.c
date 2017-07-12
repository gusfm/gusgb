#include "timer.h"
#include <stdio.h>
#include "interrupt.h"

gb_timer_t TIMER;

#define TIMER_ENABLE (1 << 2)
#define TIMER_SPEED (0x3)
static unsigned int clocks[] = {1024, 16, 64, 256};

void timer_init(void)
{
    TIMER.div = 0;
    TIMER.tima = 0;
    TIMER.tma = 0;
    TIMER.tac = 0;
    TIMER.clock.main = 0;
    TIMER.clock.internal = 0;
    TIMER.clock.speed = 1;
    TIMER.clock.select = clocks[0];
}

void timer_step(uint32_t clock_step)
{
    TIMER.clock.main += clock_step;
    TIMER.clock.internal += clock_step;
    /* Check whether a step needs to be made in the timer. */
    if (TIMER.tac & TIMER_ENABLE) {
        while (TIMER.clock.main >= TIMER.clock.select) {
            TIMER.clock.main -= TIMER.clock.select;
            if (TIMER.tima == 0xff) {
                /* If TIMA overflows, load it with TMA and generate interrupt.
                 */
                TIMER.tima = TIMER.tma;
                interrupt_raise(INTERRUPTS_TIMER);
            } else {
                TIMER.tima++;
            }
        }
    }
}

uint8_t timer_read_byte(uint16_t addr)
{
    switch (addr) {
        case 0xFF04:
            return (TIMER.clock.internal & 0xff00) >> 8;
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
            switch (TIMER.tac & 3) {
                case 0:
                    /* 4096 Hz mode bit 9 change from 1 to 0. */
                    if (TIMER.clock.internal & 0x200)
                        TIMER.tima++;
                    break;
                case 1:
                    /* 262144 Hz mode bit 3 change from 1 to 0. */
                    if (TIMER.clock.internal & 0x8)
                        TIMER.tima++;
                    break;
                case 2:
                    /* 65536 Hz mode bit 5 change from 1 to 0. */
                    if (TIMER.clock.internal & 0x20)
                        TIMER.tima++;
                    break;
                case 3:
                    /* 16384 Hz mode bit 7 change from 1 to 0. */
                    if (TIMER.clock.internal & 0x80)
                        TIMER.tima++;
                    break;
                default:
                    break;
            }
            TIMER.clock.internal = 0;
            TIMER.clock.main = 0;
            break;
        case 0xFF05:
            TIMER.tima = val;
            break;
        case 0xFF06:
            TIMER.tma = val;
            break;
        case 0xFF07:
            TIMER.clock.select = clocks[val & 3] * TIMER.clock.speed;
            TIMER.tac = val & 7;
            break;
        default:
            printf("%s: not implemented: 0x%04x!\n", __func__, addr);
            break;
    }
}

void timer_change_speed(unsigned int speed)
{
    TIMER.clock.speed = speed + 1;
}
