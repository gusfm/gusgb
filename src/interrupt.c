#include "interrupt.h"
#include <stdio.h>
#include "cpu.h"
#include "cpu_opcodes.h"

interrupt_t INTERRUPT;
extern cpu_t g_cpu;

void interrupt_init(void)
{
    INTERRUPT.master = 1;
    INTERRUPT.enable = 0;
    INTERRUPT.flags = 0;
}

uint8_t interrupt_is_enable(uint8_t bit)
{
    return INTERRUPT.enable & bit;
}

uint8_t interrupt_get_enable(void)
{
    return INTERRUPT.enable;
}

void interrupt_set_enable(uint8_t value)
{
    INTERRUPT.enable = value;
}

uint8_t interrupt_get_master(void)
{
    return INTERRUPT.master;
}

void interrupt_set_master(uint8_t value)
{
    INTERRUPT.master = value;
}

uint8_t interrupt_get_flag(void)
{
    return INTERRUPT.flags;
}

void interrupt_set_flag(uint8_t value)
{
    INTERRUPT.flags = value;
}

void interrupt_raise(uint8_t bit)
{
    INTERRUPT.flags |= bit;
    if (INTERRUPT.enable & bit)
        g_cpu.halt = false;
}

void interrupt_clear_flag_bit(uint8_t bit)
{
    INTERRUPT.flags = (uint8_t)(INTERRUPT.flags & ~bit);
}

static void vblank(void)
{
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x40;
    g_cpu.clock.step += 12;
}

static void lcd_stat(void)
{
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x48;
    g_cpu.clock.step += 12;
}

static void timer(void)
{
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x50;
    g_cpu.clock.step += 12;
}

static void serial(void)
{
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x58;
    g_cpu.clock.step += 12;
}

static void joypad(void)
{
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x60;
    g_cpu.clock.step += 12;
}

void interrupt_step(void)
{
    if (INTERRUPT.master) {
        unsigned char fire = INTERRUPT.enable & INTERRUPT.flags;
        if (fire) {
            INTERRUPT.master = 0;
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
    printf("master=0x%.2x\n", INTERRUPT.master);
    printf("enable=0x%.2x\n", INTERRUPT.enable);
    printf("flags=0x%.2x\n", INTERRUPT.flags);
}
