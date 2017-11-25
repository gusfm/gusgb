#include "clock.h"
#include "timer.h"

unsigned int step;

void clock_init(void)
{
    timer_init();
}

void clock_step(unsigned int cycles)
{
    timer_step(cycles);
    step += cycles;
}

unsigned int clock_get_step(void)
{
    return step;
}

void clock_clear(void)
{
    step = 0;
}
