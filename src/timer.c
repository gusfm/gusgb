#include "timer.h"
#include <stdio.h>
#include "debug.h"
#include "interrupt.h"

#define TIMER_ENABLE (1 << 2)

unsigned int clk_main; /* Counts how many clocks since last timer increment. */
unsigned int clk_select; /* How many clocks to increment timer. */
unsigned int clk_speed;  /* Clock speed: 1 or 2. */
uint16_t clk_sys;        /* Internal timer clock. */
uint8_t tima;            /* [$ff05] Timer counter (R/W) */
uint8_t tma;             /* [$ff06] Timer Modulo (R/W) */
uint8_t tac;             /* [$ff07] Timer Control (R/W) */
static unsigned int clocks[] = {1024, 16, 64, 256};

void timer_init(void)
{
    tima = 0;
    tma = 0;
    tac = 0;
    clk_main = 0;
    clk_sys = 0;
    clk_speed = 1;
    clk_select = clocks[0];
}

void timer_step(uint32_t clock_step)
{
    clk_main += clock_step;
    clk_sys += clock_step;
    /* Check whether a step needs to be made in the timer. */
    while (clk_main >= clk_select) {
        clk_main -= clk_select;
        if (tac & TIMER_ENABLE) {
            if (tima == 0xff) {
                /* If TIMA overflows, load it with TMA and raise interrupt. */
                tima = tma;
                interrupt_raise(INTERRUPTS_TIMER);
            } else {
                ++tima;
            }
        }
    }
}

uint8_t timer_read_byte(uint16_t addr)
{
    switch (addr) {
        case 0xFF04:
            return clk_sys >> 8;
        case 0xFF05:
            return tima;
        case 0xFF06:
            return tma;
        case 0xFF07:
            return tac;
    }
    error("not implemented: addr=0x%04x", addr);
}

void timer_write_byte(uint16_t addr, uint8_t val)
{
    switch (addr) {
        case 0xFF04:
            switch (tac & 3) {
                case 0:
                    /* 4096 Hz mode bit 9 change from 1 to 0. */
                    if (clk_sys & 0x200)
                        ++tima;
                    break;
                case 1:
                    /* 262144 Hz mode bit 3 change from 1 to 0. */
                    if (clk_sys & 0x8)
                        ++tima;
                    break;
                case 2:
                    /* 65536 Hz mode bit 5 change from 1 to 0. */
                    if (clk_sys & 0x20)
                        ++tima;
                    break;
                case 3:
                    /* 16384 Hz mode bit 7 change from 1 to 0. */
                    if (clk_sys & 0x80)
                        ++tima;
                    break;
            }
            clk_sys = 0;
            clk_main = 0;
            break;
        case 0xFF05:
            tima = val;
            break;
        case 0xFF06:
            tma = val;
            break;
        case 0xFF07:
            clk_select = clocks[val & 3] * clk_speed;
            tac = val;
            break;
        default:
            error("not implemented: 0x%04x=0x%02x", addr, val);
            break;
    }
}

void timer_change_speed(unsigned int speed)
{
    clk_speed = speed + 1;
}

void timer_dump(void)
{
    printf("Timer dump:\n");
    printf("[$ff04] div=0x%.2x\n", timer_read_byte(0xff04));
    printf("[$ff05] tima=0x%.2x\n", timer_read_byte(0xff05));
    printf("[$ff06] tma=0x%.2x\n", timer_read_byte(0xff06));
    printf("[$ff07] tac=0x%.2x\n", timer_read_byte(0xff07));
}
