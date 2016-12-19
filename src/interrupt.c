#include "interrupt.h"

static interrupt_t g_interrupt;

void interrupt_init(void)
{
    g_interrupt.master = 0;
    g_interrupt.enable = 0;
    g_interrupt.flags = 0;
}

uint8_t interrupt_enable_read(void)
{
    return g_interrupt.enable;
}

void interrupt_enable_write(uint8_t value)
{
    g_interrupt.enable = value;
}

uint8_t interrupt_flag_read(void)
{
    return g_interrupt.flags;
}
void interrupt_flag_write(uint8_t value)
{
    g_interrupt.flags = value;
}
