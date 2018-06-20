#include "wave_ch.h"
#include <string.h>
#include "timer.h"

extern apu_timer_t frame_sequencer;

void wave_ch_reset(wave_ch_t *c)
{
    memset(c, 0, sizeof(*c));
}

void wave_ch_tick(wave_ch_t *c, unsigned int clock_step)
{
    c->timer -= clock_step;
    if (c->timer <= 0) {
        c->timer += (2048 - c->frequency) * 4;
        c->position = (c->position + 1) & 0x1f;
        if (c->volume) {
            uint8_t out = c->table[c->position / 2];
            if (!(c->position & 1)) {
                out >>= 4;
            }
            c->out_volume = (out & 0xf) >> (c->volume - 1);
        } else {
            c->out_volume = 0;
        }
    }
}

bool wave_ch_status(wave_ch_t *c)
{
    return c->enabled;
}

int wave_ch_output(wave_ch_t *c)
{
    if (c->enabled)
        return c->out_volume - 8;
    else
        return 0;
}

void wave_ch_length_counter(wave_ch_t *c)
{
    length_counter_tick(&c->length, &c->enabled);
}

uint8_t wave_ch_read_reg0(wave_ch_t *c)
{
    return (c->dac_enabled << 7) | 0x7f;
}

void wave_ch_write_reg0(wave_ch_t *c, uint8_t val)
{
    c->dac_enabled = val >> 7;
    if (!c->dac_enabled)
        c->enabled = false;
}

uint8_t wave_ch_read_reg1(wave_ch_t *c)
{
    (void)c;
    return 0xff;
}

void wave_ch_write_reg1(wave_ch_t *c, uint8_t val)
{
    c->length.counter = 0x100 - val;
}

uint8_t wave_ch_read_reg2(wave_ch_t *c)
{
    return (c->volume << 5) | 0x9f;
}

void wave_ch_write_reg2(wave_ch_t *c, uint8_t val)
{
    c->volume = (val >> 5) & 3;
}

uint8_t wave_ch_read_reg3(wave_ch_t *c)
{
    (void)c;
    return 0xff;
}

void wave_ch_write_reg3(wave_ch_t *c, uint8_t val)
{
    c->frequency = (c->frequency & 0x700) | val;
    c->timer = (2048 - c->frequency) * 4;
}

uint8_t wave_ch_read_reg4(wave_ch_t *c)
{
    return c->length.enabled | 0xbf;
}

void wave_ch_write_reg4(wave_ch_t *c, uint8_t val)
{
    bool old_length_en = c->length.enabled;
    c->frequency = ((val & 0x7) << 8) | (c->frequency & 0xff);
    c->timer = (2048 - c->frequency) * 4;
    c->length.enabled = val & 0x40;
    if (!old_length_en && frame_sequencer.out_clock & 1) {
        wave_ch_length_counter(c);
    }
    if (val & 0x80) {
        if (c->dac_enabled)
            c->enabled = true;
        c->position = 0;
        if (c->length.counter == 0) {
            c->length.counter = 0x100;
            if (frame_sequencer.out_clock & 1)
                wave_ch_length_counter(c);
        }
    }
}

uint8_t wave_ch_read_table(wave_ch_t *c, int pos)
{
    return c->table[pos];
}

void wave_ch_write_table(wave_ch_t *c, int pos, uint8_t val)
{
    c->table[pos] = val;
}
