#include "timer.h"
#include <stdio.h>
#include "interrupt.h"

#define TIMER_ENABLE (1 << 2)

typedef struct {
    uint32_t main;     /* Clock used for timer count. */
    uint16_t internal; /* Internal timer clock. */
    unsigned int select;
    unsigned int speed;
} gb_clock_t;

/* TIMA states. */
typedef enum {
    TIMA_STATE_COUNTING = 0, /* Normal operation */
    TIMA_STATE_OVERFLOW,     /* Overflow happened. */
    TIMA_STATE_RELOADING,    /* TIMA is being reloaded. */
} tima_state_t;

uint8_t div_;             /* [$ff04] Divider Register (R/W) */
uint8_t tima_;            /* [$ff05] Timer counter (R/W) */
uint8_t tma_;             /* [$ff06] Timer Modulo (R/W) */
uint8_t tac_;             /* [$ff07] Timer Control (R/W) */
gb_clock_t clock_;        /* Internal timer state */
tima_state_t tima_state_; /* TIMA operation states. */
static unsigned int clocks[] = {1024, 16, 64, 256};

void timer_init(void)
{
    div_ = 0;
    tima_ = 0;
    tma_ = 0;
    tac_ = 0;
    clock_.main = 0;
    clock_.internal = 0;
    clock_.speed = 1;
    clock_.select = clocks[0];
    tima_state_ = TIMA_STATE_COUNTING;
}

void timer_step(uint32_t clock_step)
{
    clock_.main += clock_step;
    clock_.internal += clock_step;
    /* Check whether a step needs to be made in the timer. */
    if (tac_ & TIMER_ENABLE) {
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
        while (clock_.main >= clock_.select) {
            clock_.main -= clock_.select;
            if (tima_ == 0xff) {
                /* When TIMA overflows, it contains zero for 1 cycle. */
                tima_ = 0;
                tima_state_ = TIMA_STATE_OVERFLOW;
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
            return (clock_.internal & 0xff00) >> 8;
        case 0xFF05:
            return tima_;
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
            /* Increase tima when writing to div triggers a bit flip. */
            switch (tac_ & 3) {
                case 0:
                    /* 4096 Hz mode bit 9 change from 1 to 0. */
                    if (clock_.internal & 0x200)
                        ++tima_;
                    break;
                case 1:
                    /* 262144 Hz mode bit 3 change from 1 to 0. */
                    if (clock_.internal & 0x8)
                        ++tima_;
                    break;
                case 2:
                    /* 65536 Hz mode bit 5 change from 1 to 0. */
                    if (clock_.internal & 0x20)
                        ++tima_;
                    break;
                case 3:
                    /* 16384 Hz mode bit 7 change from 1 to 0. */
                    if (clock_.internal & 0x80)
                        ++tima_;
                    break;
                default:
                    break;
            }
            clock_.internal = 0;
            clock_.main = 0;
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
            clock_.select = clocks[val & 3] * clock_.speed;
            tac_ = val & 7;
            break;
        default:
            printf("%s: not implemented: 0x%04x!\n", __func__, addr);
            break;
    }
}

void timer_change_speed(unsigned int speed)
{
    clock_.speed = speed + 1;
}

void timer_dump(void)
{
    printf("Timer dump:\n");
    printf("[$ff04] div=0x%.2x\n", timer_read_byte(0xff04));
    printf("[$ff05] tima=0x%.2x\n", timer_read_byte(0xff05));
    printf("[$ff06] tma=0x%.2x\n", timer_read_byte(0xff06));
    printf("[$ff07] tac=0x%.2x\n", timer_read_byte(0xff07));
}
