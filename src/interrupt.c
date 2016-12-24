#include "interrupt.h"

interrupt_t g_interrupt;

void interrupt_init(void)
{
    g_interrupt.master = true;
    g_interrupt.enable = false;
    g_interrupt.flags = 0;
}

uint8_t interrupt_get_enable(void)
{
    return g_interrupt.enable;
}

void interrupt_set_enable(uint8_t value)
{
    g_interrupt.enable = value;
}

uint8_t interrupt_get_master(void)
{
    return g_interrupt.master;
}

void interrupt_set_master(bool value)
{
    g_interrupt.master = value;
}

uint8_t interrupt_flag_read(void)
{
    return g_interrupt.flags;
}
void interrupt_flag_write(uint8_t value)
{
    g_interrupt.flags = value;
}
