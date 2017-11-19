#include "timer.h"
#include <stdio.h>
#include "debug.h"
#include "interrupt.h"

#define TIMER_ENABLE (1 << 2)

unsigned int clk_main_; /* Counts how many clocks since last timer increment. */
unsigned int clk_select_; /* How many clocks to increment timer. */
unsigned int clk_speed_;  /* Clock speed: 1 or 2. */
uint16_t clk_sys_;        /* Internal timer clock. */
uint8_t tima_;            /* [$ff05] Timer counter (R/W) */
uint8_t tma_;             /* [$ff06] Timer Modulo (R/W) */
uint8_t tac_;             /* [$ff07] Timer Control (R/W) */
static unsigned int clocks[] = {1024, 16, 64, 256};

void timer_init(void)
{
    tima_ = 0;
    tma_ = 0;
    tac_ = 0;
    clk_main_ = 0;
    clk_sys_ = 0;
    clk_speed_ = 1;
    clk_select_ = clocks[0];
}

void timer_step(uint32_t clock_step)
{
    clk_main_ += clock_step;
    clk_sys_ += clock_step;
    /* Check whether a step needs to be made in the timer. */
    while (clk_main_ >= clk_select_) {
        clk_main_ -= clk_select_;
        if (tac_ & TIMER_ENABLE) {
            if (tima_ == 0xff) {
                /* If TIMA overflows, load it with TMA and raise interrupt. */
                tima_ = tma_;
                interrupt_raise(INTERRUPTS_TIMER);
            } else {
                ++tima_;
            }
        }
    }
}

uint8_t timer_read_byte(uint16_t addr)
{
    switch (addr) {
        case 0xFF04:
            return clk_sys_ >> 8;
        case 0xFF05:
            return tima_;
        case 0xFF06:
            return tma_;
        case 0xFF07:
            return tac_;
    }
    error("not implemented: addr=0x%04x", addr);
}

void timer_write_byte(uint16_t addr, uint8_t val)
{
    switch (addr) {
        case 0xFF04:
            switch (tac_ & 3) {
                case 0:
                    /* 4096 Hz mode bit 9 change from 1 to 0. */
                    if (clk_sys_ & 0x200)
                        ++tima_;
                    break;
                case 1:
                    /* 262144 Hz mode bit 3 change from 1 to 0. */
                    if (clk_sys_ & 0x8)
                        ++tima_;
                    break;
                case 2:
                    /* 65536 Hz mode bit 5 change from 1 to 0. */
                    if (clk_sys_ & 0x20)
                        ++tima_;
                    break;
                case 3:
                    /* 16384 Hz mode bit 7 change from 1 to 0. */
                    if (clk_sys_ & 0x80)
                        ++tima_;
                    break;
            }
            clk_sys_ = 0;
            clk_main_ = 0;
            break;
        case 0xFF05:
            tima_ = val;
            break;
        case 0xFF06:
            tma_ = val;
            break;
        case 0xFF07:
            clk_select_ = clocks[val & 3] * clk_speed_;
            tac_ = val;
            break;
        default:
            error("not implemented: 0x%04x=0x%02x", addr, val);
            break;
    }
}

void timer_change_speed(unsigned int speed)
{
    clk_speed_ = speed + 1;
}

void timer_dump(void)
{
    printf("Timer dump:\n");
    printf("[$ff04] div=0x%.2x\n", timer_read_byte(0xff04));
    printf("[$ff05] tima=0x%.2x\n", timer_read_byte(0xff05));
    printf("[$ff06] tma=0x%.2x\n", timer_read_byte(0xff06));
    printf("[$ff07] tac=0x%.2x\n", timer_read_byte(0xff07));
}
