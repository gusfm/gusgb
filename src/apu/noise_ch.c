#include "noise_ch.h"
#include <string.h>
#include "apu/timer.h"

extern apu_timer_t frame_sequencer;

void noise_ch_reset(noise_ch_t *c)
{
    memset(c, 0, sizeof(*c));
}

void noise_ch_tick(noise_ch_t *c, unsigned int clock_step)
{
}

bool noise_ch_status(noise_ch_t *c)
{
    return c->enabled;
}

int noise_ch_output(noise_ch_t *c)
{
    if (c->enabled)
        return 1;
    else
        return 0;
}

void noise_ch_length_counter(noise_ch_t *c)
{
    length_counter_tick(&c->length, &c->enabled);
}

void noise_ch_volume_envelope(noise_ch_t *c)
{
    if (c->env.period > 0 && --c->env.count <= 0) {
        c->env.count = c->env.period;
        if (c->env.direction) {
            if (c->env.volume < 0xf) {
                c->env.volume++;
            }
        } else {
            if (c->env.volume > 0) {
                c->env.volume--;
            }
        }
    }
}
uint8_t noise_ch_read_reg1(noise_ch_t *c)
{
    (void)c;
    return 0xff;
}

void noise_ch_write_reg1(noise_ch_t *c, uint8_t val)
{
    c->length.counter = 64 - (val & 0x3f);
}

uint8_t noise_ch_read_reg2(noise_ch_t *c)
{
    return (c->env.volume << 4) | (c->env.direction << 3) | c->env.period;
}

void noise_ch_write_reg2(noise_ch_t *c, uint8_t val)
{
    c->env.volume = (val & 0xf0) >> 4;
    c->env.direction = (val & 0x8) >> 3;
    c->env.period = val & 0x7;
    c->env.count = c->env.period;
    /* Check if DAC is disabled */
    if (!(val & 0xf8)) {
        /* Disable channel output */
        c->enabled = false;
    }
}

uint8_t noise_ch_read_reg3(noise_ch_t *c)
{
    return (c->shift_clock << 4) | (c->counter_step << 3) | c->div_ratio;
}

void noise_ch_write_reg3(noise_ch_t *c, uint8_t val)
{
    c->shift_clock = val >> 4;
    c->counter_step = (val >> 3) & 1;
    c->div_ratio = val & 7;
}

uint8_t noise_ch_read_reg4(noise_ch_t *c)
{
    return c->length.enabled | 0xbf;
}

static void noise_ch_trigger(noise_ch_t *c)
{
    /* Only enable output if DAC is enabled. */
    if (c->env.volume | c->env.direction)
        c->enabled = true;
    if (c->length.counter == 0) {
        c->length.counter = 64;
        if (frame_sequencer.out_clock & 1)
            noise_ch_length_counter(c);
    }
}

void noise_ch_write_reg4(noise_ch_t *c, uint8_t val)
{
    uint8_t old_length_en = c->length.enabled;
    c->length.enabled = val & 0x40;
    if (!old_length_en && frame_sequencer.out_clock & 1) {
        noise_ch_length_counter(c);
    }
    if (val & 0x80) {
        noise_ch_trigger(c);
    }
}
