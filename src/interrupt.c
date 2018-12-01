#include "interrupt.h"
#include <stdio.h>
#include "clock.h"
#include "cpu.h"
#include "cpu_opcodes.h"

extern cpu_t CPU;

static bool ime;             /* Interrupt master enable: IE, DI */
static unsigned int ime_cnt; /* IE takes efect after next instr. */
static unsigned int enable;  /* Interrupt enable: 0xffff register */
static unsigned int flag;    /* Interrupt flag: 0xff0f register */

void interrupt_reset(void)
{
    ime = true;
    ime_cnt = 0;
    enable = 0;
    flag = 1;
}

void interrupt_set_master(bool value)
{
    if (!ime)
        ime_cnt = 0;
    ime = value;
}

uint8_t interrupt_is_enable(uint8_t bit)
{
    return enable & bit;
}

uint8_t interrupt_get_enable(void)
{
    return enable;
}

void interrupt_set_enable(uint8_t value)
{
    enable = value;
}

uint8_t interrupt_get_flag(void)
{
    return 0xe0 | flag;
}

void interrupt_set_flag(uint8_t value)
{
    flag = 0x1f & value;
}

void interrupt_raise(uint8_t bit)
{
    flag |= bit;
}

static inline void interrupt_clear_flag_bit(uint8_t bit)
{
    flag = (uint8_t)(flag & ~bit);
}

void interrupt_step(void)
{
    unsigned char fire = enable & flag;
    ++ime_cnt;
    if (fire) {
        if (CPU.halt && !CPU.halt_bug) {
            CPU.halt = false;
            CPU.reg.pc++;
        }
        if (!ime || ime_cnt < 2)
            return;
        ime = false;
        push(CPU.reg.pc);
        clock_step(12);
        if (fire & INTERRUPTS_VBLANK) {
            interrupt_clear_flag_bit(INTERRUPTS_VBLANK);
            CPU.reg.pc = 0x40;
            return;
        }
        if (fire & INTERRUPTS_LCDSTAT) {
            interrupt_clear_flag_bit(INTERRUPTS_LCDSTAT);
            CPU.reg.pc = 0x48;
            return;
        }
        if (fire & INTERRUPTS_TIMER) {
            interrupt_clear_flag_bit(INTERRUPTS_TIMER);
            CPU.reg.pc = 0x50;
            return;
        }
        if (fire & INTERRUPTS_SERIAL) {
            interrupt_clear_flag_bit(INTERRUPTS_SERIAL);
            CPU.reg.pc = 0x58;
            return;
        }
        if (fire & INTERRUPTS_JOYPAD) {
            interrupt_clear_flag_bit(INTERRUPTS_JOYPAD);
            CPU.reg.pc = 0x60;
            return;
        }
    }
}

void interrupt_dump(void)
{
    printf("Interrupts:\n");
    printf("ime=%d\n", ime);
    printf("ie=0x%.2x\n", enable);
    printf("if=0x%.2x\n", flag);
}
