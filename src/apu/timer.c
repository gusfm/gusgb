#include "apu/timer.h"
#include <stdlib.h>

void apu_timer_init(apu_timer_t *t, unsigned int freq, unsigned int sum,
                    unsigned int mask, apu_timer_cb_f cb)
{
    t->freq = freq;
    t->sum = sum;
    t->mask = mask;
    t->cb = cb;
    t->in_clock = 0;
    t->out_clock = 0;
}

void apu_timer_tick(apu_timer_t *t, unsigned int cycles)
{
    t->in_clock += cycles;
    if (t->in_clock >= t->freq) {
        t->in_clock -= t->freq;
        t->cb(t->out_clock);
        t->out_clock = (t->out_clock + t->sum) & t->mask;
    }
}
