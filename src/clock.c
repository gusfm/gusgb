#include "clock.h"

unsigned int step;

void clock_step(unsigned int cycles)
{
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
