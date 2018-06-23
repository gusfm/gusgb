#include "sqr_ch.h"
#include <string.h>
#include "timer.h"

extern apu_timer_t frame_sequencer;
extern uint8_t sound_enabled;

static uint8_t duty_table[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1}, /* 12.5% */
    {1, 0, 0, 0, 0, 0, 0, 1}, /* 25% */
    {1, 0, 0, 0, 0, 1, 1, 1}, /* 50% */
    {0, 1, 1, 1, 1, 1, 1, 0}  /* 75% */
};

void sqr_ch_reset(sqr_ch_t *c)
{
    memset(c, 0, sizeof(*c));
}

void sqr_ch_tick(sqr_ch_t *c)
{
    if (--c->timer <= 0) {
        c->timer = (2048 - c->frequency) * 4;
        c->wave_ptr = (c->wave_ptr + 1) & 0x7;
    }
}

bool sqr_ch_status(sqr_ch_t *c)
{
    return c->enabled;
}

int sqr_ch_output(sqr_ch_t *c)
{
    if (c->enabled)
        return duty_table[c->wave_duty][c->wave_ptr] * c->env.volume - 8;
    else
        return 0;
}

static unsigned int sweep_calc(sqr_ch_t *c)
{
    unsigned int freq = (c->sweep.frequency >> c->sweep.shift);
    if (c->sweep.negate) {
        freq = c->sweep.frequency - freq;
        c->sweep.neg_calc = true;
    } else {
        freq = c->sweep.frequency + freq;
    }
    if (freq > 2047)
        c->enabled = false;
    return freq;
}

void sqr_ch_sweep(sqr_ch_t *c)
{
    if (--c->sweep.period_tmp <= 0) {
        c->sweep.period_tmp = c->sweep.period == 0 ? 8 : c->sweep.period;
        if (c->sweep.enabled && c->sweep.period > 0) {
            unsigned int freq = sweep_calc(c);
            if (freq <= 2047 && c->sweep.shift > 0) {
                c->frequency = freq;
                c->sweep.frequency = freq;
                sweep_calc(c);
            }
        }
    }
}

void sqr_ch_length_counter(sqr_ch_t *c)
{
    length_counter_tick(&c->length, &c->enabled);
}

void sqr_ch_volume_envelope(sqr_ch_t *c)
{
    if (c->enabled)
        volume_envelope_tick(&c->env);
}

uint8_t sqr_ch_read_reg0(sqr_ch_t *c)
{
    return 0x80 | (c->sweep.period << 4) | (c->sweep.negate << 3) |
           c->sweep.shift;
}

void sqr_ch_write_reg0(sqr_ch_t *c, uint8_t val)
{
    c->sweep.period = val >> 4;
    c->sweep.negate = (val >> 3) & 1;
    c->sweep.shift = val & 0x7;
    if (c->sweep.neg_calc && !c->sweep.negate) {
        c->enabled = false;
        c->sweep.neg_calc = 0;
    }
}

uint8_t sqr_ch_read_reg1(sqr_ch_t *c)
{
    return (c->wave_duty << 6) | 0x3f;
}

void sqr_ch_write_reg1(sqr_ch_t *c, uint8_t val)
{
    if (sound_enabled)
        c->wave_duty = val >> 6;
    c->length.counter = 64 - (val & 0x3f);
}

uint8_t sqr_ch_read_reg2(sqr_ch_t *c)
{
    return volume_envelope_read(&c->env);
}

void sqr_ch_write_reg2(sqr_ch_t *c, uint8_t val)
{
    volume_envelope_write(&c->env, val, &c->enabled);
}

uint8_t sqr_ch_read_reg3(sqr_ch_t *c)
{
    (void)c;
    return 0xff;
}

void sqr_ch_write_reg3(sqr_ch_t *c, uint8_t val)
{
    c->frequency = (c->frequency & 0x700) | val;
}

uint8_t sqr_ch_read_reg4(sqr_ch_t *c)
{
    return c->length.enabled | 0xbf;
}

static void sqr_ch_sweep_init(sqr_ch_t *c)
{
    c->sweep.neg_calc = false;
    c->sweep.frequency = c->frequency;
    c->sweep.period_tmp = (c->sweep.period == 0) ? 8 : c->sweep.period;
    c->sweep.enabled = (c->sweep.period | c->sweep.shift) ? 1 : 0;
    if (c->sweep.shift > 0) {
        sweep_calc(c);
    }
}

static void sqr_ch_trigger(sqr_ch_t *c)
{
    /* Only enable output if DAC is enabled. */
    if (c->env.volume | c->env.direction)
        c->enabled = true;
    if (c->length.counter == 0) {
        c->length.counter = 64;
        if (frame_sequencer.out_clock & 1)
            sqr_ch_length_counter(c);
    }
    c->timer = (2048 - c->frequency) * 4;
    c->wave_ptr = 0;
    sqr_ch_sweep_init(c);
}

void sqr_ch_write_reg4(sqr_ch_t *c, uint8_t val)
{
    uint8_t old_length_en = c->length.enabled;
    c->length.enabled = val & 0x40;
    if (!old_length_en && frame_sequencer.out_clock & 1) {
        sqr_ch_length_counter(c);
    }
    c->frequency = ((val & 0x7) << 8) | (c->frequency & 0xff);
    if (val & 0x80) {
        sqr_ch_trigger(c);
    }
}
