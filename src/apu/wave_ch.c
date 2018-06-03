#include "wave_ch.h"
#include <string.h>

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
        if (c->enabled && c->status && c->volume) {
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

uint8_t wave_ch_status(wave_ch_t *c)
{
    return c->status;
}

int16_t wave_ch_output(wave_ch_t *c)
{
    return c->out_volume;
}

void wave_ch_length_counter(wave_ch_t *c)
{
    length_counter_tick(&c->length, &c->status);
}

uint8_t wave_ch_read_reg0(wave_ch_t *c)
{
    return (c->enabled << 7) | 0x7f;
}

void wave_ch_write_reg0(wave_ch_t *c, uint8_t val)
{
    c->enabled = val >> 7;
    if (!c->enabled) {
        c->status = false;
    }
}

uint8_t wave_ch_read_reg1(wave_ch_t *c)
{
    return c->length.counter;
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
    c->frequency = ((val & 0x7) << 8) | (c->frequency & 0xff);
    c->timer = (2048 - c->frequency) * 4;
    c->length.enabled = val & 0x40;
    if (val & 0x80) {
        if (c->enabled)
            c->status = true;
        c->position = 0;
        if (c->length.counter == 0) {
            c->length.counter = 0x100;
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
