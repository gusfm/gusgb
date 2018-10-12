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
    ime = value;
    ime_cnt = 0;
}

bool interrupt_master_enabled(void)
{
    return ime;
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
    if (enable & bit)
        CPU.halt = false;
}

void interrupt_clear_flag_bit(uint8_t bit)
{
    flag = (uint8_t)(flag & ~bit);
}

static void vblank(void)
{
    push(CPU.reg.pc);
    CPU.reg.pc = 0x40;
    clock_step(12);
}

static void lcd_stat(void)
{
    push(CPU.reg.pc);
    CPU.reg.pc = 0x48;
    clock_step(12);
}

static void timer(void)
{
    push(CPU.reg.pc);
    CPU.reg.pc = 0x50;
    clock_step(12);
}

static void serial(void)
{
    push(CPU.reg.pc);
    CPU.reg.pc = 0x58;
    clock_step(12);
}

static void joypad(void)
{
    push(CPU.reg.pc);
    CPU.reg.pc = 0x60;
    clock_step(12);
}

void interrupt_step(void)
{
    if (ime) {
        if (ime_cnt == 0) {
            ++ime_cnt;
            return;
        }
        unsigned char fire = enable & flag;
        if (fire) {
            interrupt_set_master(false);
            if (fire & INTERRUPTS_VBLANK) {
                interrupt_clear_flag_bit(INTERRUPTS_VBLANK);
                vblank();
            }
            if (fire & INTERRUPTS_LCDSTAT) {
                interrupt_clear_flag_bit(INTERRUPTS_LCDSTAT);
                lcd_stat();
            }
            if (fire & INTERRUPTS_TIMER) {
                interrupt_clear_flag_bit(INTERRUPTS_TIMER);
                timer();
            }
            if (fire & INTERRUPTS_SERIAL) {
                interrupt_clear_flag_bit(INTERRUPTS_SERIAL);
                serial();
            }
            if (fire & INTERRUPTS_JOYPAD) {
                interrupt_clear_flag_bit(INTERRUPTS_JOYPAD);
                joypad();
            }
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
