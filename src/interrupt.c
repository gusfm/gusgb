#include <stdio.h>
#include "interrupt.h"
#include "cpu.h"
#include "cpu_opcodes.h"

interrupt_t g_interrupt;
extern cpu_t g_cpu;

void interrupt_init(void)
{
    g_interrupt.master = 1;
    g_interrupt.enable = 0;
    g_interrupt.flags = 0;
}

uint8_t interrupt_is_enable(uint8_t bit)
{
    return g_interrupt.enable & bit;
}

uint8_t interrupt_get_enable(void)
{
    printf("%s:%d: enable=0x%02x\n", __func__, __LINE__, g_interrupt.enable);
    return g_interrupt.enable;
}

void interrupt_set_enable(uint8_t value)
{
    printf("%s:%d: enable=0x%02x\n", __func__, __LINE__, value);
    g_interrupt.enable = value;
}

uint8_t interrupt_get_master(void)
{
    printf("%s:%d: master=0x%02x\n", __func__, __LINE__, g_interrupt.master);
    return g_interrupt.master;
}

void interrupt_set_master(uint8_t value)
{
    printf("%s:%d: master=0x%02x\n", __func__, __LINE__, value);
    g_interrupt.master = value;
}

uint8_t interrupt_get_flag(void)
{
    printf("%s:%d: value=0x%02x\n", __func__, __LINE__, g_interrupt.flags);
    return g_interrupt.flags;
}

void interrupt_set_flag(uint8_t value)
{
    g_interrupt.flags = value;
    printf("%s:%d: flags=0x%02x\n", __func__, __LINE__, g_interrupt.flags);
}

void interrupt_set_flag_bit(uint8_t bit)
{
    g_interrupt.flags |= bit;
    printf("%s:%d: flags=0x%02x\n", __func__, __LINE__, g_interrupt.flags);
}

void interrupt_clear_flag_bit(uint8_t bit)
{
    g_interrupt.flags = (uint8_t)(g_interrupt.flags & ~bit);
}

static void vblank(void)
{
    printf("%s:%d\n", __func__, __LINE__);
    g_interrupt.master = 0;
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x40;
    g_cpu.clock.step += 3;
}

static void lcd_stat(void)
{
    printf("%s:%d\n", __func__, __LINE__);
    g_interrupt.master = 0;
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x48;
    g_cpu.clock.step += 3;
}

static void timer(void)
{
    printf("%s:%d\n", __func__, __LINE__);
    g_interrupt.master = 0;
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x50;
    g_cpu.clock.step += 3;
}

static void serial(void)
{
    printf("%s:%d\n", __func__, __LINE__);
    g_interrupt.master = 0;
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x58;
    g_cpu.clock.step += 3;
}

static void joypad(void)
{
    printf("%s:%d\n", __func__, __LINE__);
    g_interrupt.master = 0;
    push(g_cpu.reg.pc);
    g_cpu.reg.pc = 0x60;
    g_cpu.clock.step += 3;
}

void interrupt_step(void)
{
    if (g_interrupt.master && g_interrupt.enable && g_interrupt.flags) {
        unsigned char fire = g_interrupt.enable & g_interrupt.flags;

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
