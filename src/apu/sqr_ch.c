#include "sqr_ch.h"
#include <string.h>
#include "apu/timer.h"

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

uint8_t sqr_ch_read_reg1(sqr_ch_t *c)
{
    return (c->wave_duty << 6) | 0x3f;
}

void sqr_ch_write_reg1(sqr_ch_t *c, uint8_t val)
{
    c->wave_duty = val >> 6;
    c->length_counter = 64 - (val & 0x3f);
}

uint8_t sqr_ch_read_reg2(sqr_ch_t *c)
{
    return (c->env_volume << 4) | (c->env_direction << 3) | c->env_period;
}

void sqr_ch_write_reg2(sqr_ch_t *c, uint8_t val)
{
    c->env_volume = (val & 0xf0) >> 4;
    c->env_direction = (val & 0x8) >> 3;
    c->env_period = val & 0x7;
    c->env_count = c->env_period;
}

uint8_t sqr_ch_read_reg3(sqr_ch_t *c)
{
    (void)c;
    return 0xff;
}

void sqr_ch_write_reg3(sqr_ch_t *c, uint8_t val)
{
    c->frequency = (c->frequency & 0x700) | val;
    c->timer = (2048 - c->frequency) * 4;
}

uint8_t sqr_ch_read_reg4(sqr_ch_t *c)
{
    return c->length_en | 0xbf;
}

uint8_t sqr_ch_status(sqr_ch_t *c)
{
    return c->status;
}

void sqr_ch_write_reg4(sqr_ch_t *c, uint8_t val)
{
    c->status = (val & 0x80) != 0;
    c->length_en = (val & 0x40) != 0;
    c->frequency = ((val & 0x7) << 8) | (c->frequency & 0xff);
    c->timer = (2048 - c->frequency) * 4;
    c->wave_ptr = 0;
    if (c->status && c->length_counter == 0) {
        c->length_counter = 64;
    }
}

void sqr_ch_length_counter(sqr_ch_t *c)
{
    if (c->length_en && c->length_counter > 0) {
        if (--c->length_counter == 0) {
            /* Disable output */
            c->status = 0;
        }
    }
}

void sqr_ch_volume_envelope(sqr_ch_t *c)
{
    if (c->env_period > 0 && --c->env_count <= 0) {
        c->env_count = c->env_period;
        if (c->env_direction) {
            if (c->env_volume < 0xf) {
                c->env_volume++;
            }
        } else {
            if (c->env_volume > 0) {
                c->env_volume++;
            }
        }
    }
}

int16_t sqr_ch_output(sqr_ch_t *c)
{
    int16_t val = c->status & duty_table[c->wave_duty][c->wave_ptr];
    return val * c->env_volume;
}

void sqr_ch_tick(sqr_ch_t *c, unsigned int clock_step)
{
    c->timer -= clock_step;
    if (c->timer <= 0) {
        c->timer += (2048 - c->frequency) * 4;
        c->wave_ptr = (c->wave_ptr + 1) & 0x7;
    }
}
