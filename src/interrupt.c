#include "interrupt.h"
#include <stdio.h>
#include "cpu.h"
#include "cpu_opcodes.h"

extern cpu_t g_cpu;

unsigned int ime_; /* Interrupt master enable: IE -> enable, DI -> disable */
unsigned int ime_cnt_; /* IE takes efect after next instruction. */
unsigned int ie_;      /* Interrupt enable: 0xffff register */
unsigned int if_;      /* Interrupt flag: 0xff0f register */

void interrupt_init(void)
{
    ime_ = 1;
    ime_cnt_ = 0;
    ie_ = 0;
    if_ = 0;
}

void interrupt_set_master(uint8_t value)
{
    ime_ = value;
    ime_cnt_ = 0;
}

uint8_t interrupt_is_enable(uint8_t bit)
{
    return ie_ & bit;
}

uint8_t interrupt_get_enable(void)
{
    return ie_;
}

void interrupt_set_enable(uint8_t value)
{
    ie_ = value;
}

uint8_t interrupt_get_flag(void)
{
    return if_;
}

void interrupt_set_flag(uint8_t value)
{
    if_ = value;
}

void interrupt_raise(uint8_t bit)
{
    if_ |= bit;
    if (ie_ & bit)
        g_cpu.halt = false;
}

void interrupt_clear_flag_bit(uint8_t bit)
{
    if_ = (uint8_t)(if_ & ~bit);
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
    if (ime_) {
        if (ime_cnt_ == 0) {
            ++ime_cnt_;
            return;
        }
        unsigned char fire = ie_ & if_;
        if (fire) {
            interrupt_set_master(0);
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
    printf("ime=0x%.2x\n", ime_);
    printf("ie=0x%.2x\n", ie_);
    printf("if=0x%.2x\n", if_);
}
