#include <stdio.h>
#include "timer.h"
#include "interrupt.h"

timer_t TIMER;

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

static void timer_inc_tima(void)
{
    TIMER.clock.main = 0;
    if (TIMER.tima == 0xff) {
        TIMER.tima = TIMER.tma;
        interrupt_set_flag_bit(INTERRUPTS_JOYPAD);
    } else {
        TIMER.tima++;
    }
}

void timer_step(uint32_t cpu_tick)
{
    TIMER.clock.sub += cpu_tick;
    if (TIMER.clock.sub > 3) {
        TIMER.clock.main++;
        TIMER.clock.sub -= 4;
        TIMER.clock.div++;
        if (TIMER.clock.div == 16) {
            TIMER.clock.div = 0;
            TIMER.div++;
        }
    }

    if (TIMER.tac & 4) {
        switch (TIMER.tac & 3) {
            case 0:
                if (TIMER.clock.main >= 64)
                    timer_inc_tima();
                break;
            case 1:
                if (TIMER.clock.main >= 1)
                    timer_inc_tima();
                break;
            case 2:
                if (TIMER.clock.main >= 4)
                    timer_inc_tima();
                break;
            case 3:
                if (TIMER.clock.main >= 16)
                    timer_inc_tima();
                break;
        }
    }
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
