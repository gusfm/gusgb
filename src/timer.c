#include "timer.h"
#include <stdio.h>
#include "interrupt.h"

#define TIMER_ENABLE (1 << 2)

/* TIMA states. */
typedef enum {
    TIMA_STATE_COUNTING = 0, /* Normal operation */
    TIMA_STATE_OVERFLOW,     /* Overflow happened. */
    TIMA_STATE_RELOADING,    /* TIMA is being reloaded. */
} tima_state_t;

uint16_t clk_sys_;        /* Internal timer clock. */
uint16_t clk_last_;       /* Last cycle clock. */
unsigned int tima_;       /* [$ff05] Timer counter (R/W) */
uint8_t tma_;             /* [$ff06] Timer Modulo (R/W) */
uint8_t tac_;             /* [$ff07] Timer Control (R/W) */
tima_state_t tima_state_; /* TIMA operation states. */
unsigned int delay_bit_;

void timer_init(void)
{
    tima_ = 0;
    tma_ = 0;
    tac_ = 0;
    clk_sys_ = 0;
    clk_last_ = 0;
    tima_state_ = TIMA_STATE_COUNTING;
    delay_bit_ = 0;
}

static unsigned int mux(uint8_t sel, uint16_t in)
{
    uint16_t mask;
    switch (sel & 3) {
        case 0:
            /* 4096 Hz mode bit 9 change from 1 to 0. */
            mask = 0x200;
            break;
        case 1:
            /* 262144 Hz mode bit 3 change from 1 to 0. */
            mask = 0x8;
            break;
        case 2:
            /* 65536 Hz mode bit 5 change from 1 to 0. */
            mask = 0x20;
            break;
        case 3:
            /* 16384 Hz mode bit 7 change from 1 to 0. */
            mask = 0x80;
            break;
    }
    return in & mask;
}

void timer_step(uint32_t clock_step)
{
    clk_last_ = clk_sys_;
    clk_sys_ += clock_step;
    /* Check whether a step needs to be made in the timer. */
    switch (tima_state_) {
        case TIMA_STATE_COUNTING:
            break;
        case TIMA_STATE_OVERFLOW:
            /* After one cycle, TIMA should be reloaded and the interrupt
             * flag set.
             */
            tima_ = tma_;
            interrupt_raise(INTERRUPTS_TIMER);
            tima_state_ = TIMA_STATE_RELOADING;
            break;
        case TIMA_STATE_RELOADING:
            tima_state_ = TIMA_STATE_COUNTING;
            break;
    }
    /* Falling edge detector. */
    unsigned int tac_enabled = tac_ & TIMER_ENABLE;
    for (uint16_t clock = clk_last_ + 1; clock <= clk_sys_; ++clock) {
        unsigned int bit = mux(tac_, clock) && tac_enabled;
        tima_ += delay_bit_ & ~bit;
        delay_bit_ = bit;
    }
    if (tima_ == 0x100) {
        /* When TIMA overflows, it contains zero for 1 cycle. */
        tima_ = 0;
        tima_state_ = TIMA_STATE_OVERFLOW;
    }
}

uint8_t timer_read_byte(uint16_t addr)
{
    switch (addr) {
        case 0xFF04:
            return (clk_sys_ & 0xff00) >> 8;
        case 0xFF05:
            return tima_ & 0xff;
        case 0xFF06:
            return tma_;
        case 0xFF07:
            return tac_;
        default:
            printf("%s: not implemented: 0x%04x!\n", __func__, addr);
            return 0;
    }
}

void timer_write_byte(uint16_t addr, uint8_t val)
{
    switch (addr) {
        case 0xFF04:
            clk_sys_ = 0;
            break;
        case 0xFF05:
            switch (tima_state_) {
                case TIMA_STATE_COUNTING:
                    /* Normal operation. */
                    tima_ = val;
                    break;
                case TIMA_STATE_OVERFLOW:
                    /* Prevent reload if writing to it during overflow cycle. */
                    tima_ = val;
                    tima_state_ = TIMA_STATE_COUNTING;
                    break;
                case TIMA_STATE_RELOADING:
                    /* Ignore writes if TIMA was just reloaded. */
                    break;
            }
            break;
        case 0xFF06:
            tma_ = val;
            if (tima_state_ == TIMA_STATE_RELOADING) {
                /* If TMA is written in the same cycle that TIMA is reloaded,
                 * write the value to TIMA as well. */
                tima_ = tma_;
            }
            break;
        case 0xFF07:
            tac_ = val;
            break;
        default:
            printf("%s: not implemented: 0x%04x!\n", __func__, addr);
            break;
    }
}

void timer_change_speed(unsigned int speed)
{
    (void)speed;
    /* TODO */
}

void timer_dump(void)
{
    printf("Timer dump:\n");
    printf("[$ff04] div=0x%.2x\n", timer_read_byte(0xff04));
    printf("[$ff05] tima=0x%.2x\n", timer_read_byte(0xff05));
    printf("[$ff06] tma=0x%.2x\n", timer_read_byte(0xff06));
    printf("[$ff07] tac=0x%.2x\n", timer_read_byte(0xff07));
}
