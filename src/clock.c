#include "clock.h"
#include "timer.h"

static unsigned int step;

void clock_reset(void)
{
    timer_reset();
}

inline void clock_step(unsigned int cycles)
{
    timer_step(cycles);
    step += cycles;
}

inline unsigned int clock_get_step(void)
{
    return step;
}

inline void clock_clear(void)
{
    step = 0;
}
