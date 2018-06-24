#include "noise_ch.h"
#include <string.h>
#include "timer.h"

extern apu_timer_t frame_sequencer;

static uint8_t divisor[] = { 8, 16, 32, 48, 64, 80, 96, 112 };

void noise_ch_reset(noise_ch_t *c)
{
    memset(c, 0, sizeof(*c));
}

void noise_ch_tick(noise_ch_t *c)
{
    c->timer -= 2;
    if (c->timer <= 0) {
        c->timer = divisor[c->div_ratio] << c->shift_clock;
        uint32_t lfsr = c->lfsr;
        uint32_t xor_result = (lfsr & 1) ^ ((lfsr >> 1) & 1);
        lfsr = (lfsr >> 1) | (xor_result << 14);
        if (c->width_mode)
            lfsr = (lfsr & ~0x40) | (xor_result << 6);
        c->lfsr = lfsr;
        if (c->enabled && c->env.dac_enabled && !(lfsr & 1))
            c->out_volume = c->env.volume;
        else
            c->out_volume = 0;
    }
}

bool noise_ch_status(noise_ch_t *c)
{
    return c->enabled;
}

int noise_ch_output(noise_ch_t *c)
{
    return c->out_volume;
}

void noise_ch_length_counter(noise_ch_t *c)
{
    length_counter_tick(&c->length, &c->enabled);
}

void noise_ch_volume_envelope(noise_ch_t *c)
{
    if (c->enabled)
        volume_envelope_tick(&c->env);
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
    return volume_envelope_read(&c->env);
}

void noise_ch_write_reg2(noise_ch_t *c, uint8_t val)
{
    volume_envelope_write(&c->env, val, &c->enabled);
}

uint8_t noise_ch_read_reg3(noise_ch_t *c)
{
    return (c->shift_clock << 4) | (c->width_mode << 3) | c->div_ratio;
}

void noise_ch_write_reg3(noise_ch_t *c, uint8_t val)
{
    c->shift_clock = val >> 4;
    c->width_mode = (val >> 3) & 1;
    c->div_ratio = val & 7;
}

uint8_t noise_ch_read_reg4(noise_ch_t *c)
{
    return c->length.enabled | 0xbf;
}

static void noise_ch_trigger(noise_ch_t *c)
{
    if (c->env.dac_enabled)
        c->enabled = true;
    if (c->length.counter == 0) {
        c->length.counter = 64;
        if (frame_sequencer.out_clock & 1)
            noise_ch_length_counter(c);
    }
    c->lfsr = 0x7fff;
    c->timer = 6 + (divisor[c->div_ratio] << c->shift_clock);
    volume_envelope_trigger(&c->env);
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
