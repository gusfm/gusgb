#ifndef APU_TIMER_H
#define APU_TIMER_H

typedef void (*apu_timer_cb_f)(unsigned int clock);

typedef struct {
    unsigned int freq;
    unsigned int sum;
    unsigned int mask;
    apu_timer_cb_f cb;
    unsigned int in_clock;
    unsigned int out_clock;
} apu_timer_t;

void apu_timer_init(apu_timer_t *t, unsigned int freq, unsigned int sum,
                    unsigned int mask, apu_timer_cb_f cb);
void apu_timer_tick(apu_timer_t *t, unsigned int cycles);

#endif /* APU_TIMER_H */
