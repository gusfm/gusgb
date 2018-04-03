#include "ch2.h"
#include <string.h>
#include "apu/timer.h"

static uint8_t wave_duty;
static unsigned int wave_ptr;
static uint8_t length_en;
static unsigned int length_counter;
static uint16_t frequency;
static uint8_t status;
static int timer;
static uint8_t env_volume;
static uint8_t env_direction;
static uint8_t env_period;
static int env_count;

static uint8_t duty_table[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1}, /* 12.5% */
    {1, 0, 0, 0, 0, 0, 0, 1}, /* 25% */
    {1, 0, 0, 0, 0, 1, 1, 1}, /* 50% */
    {0, 1, 1, 1, 1, 1, 1, 0}  /* 75% */
};

void ch2_reset(void)
{
    wave_duty = 0;
    wave_ptr = 0;
    length_counter = 0;
    frequency = 0;
    status = 0;
    length_en = 0;
    timer = 0;
    env_volume = 0;
    env_direction = 0;
    env_period = 0;
    env_count = 0;
}

uint8_t ch2_read_reg1(void)
{
    return (wave_duty << 6) | 0x3f;
}

void ch2_write_reg1(uint8_t val)
{
    wave_duty = val >> 6;
    length_counter = 64 - (val & 0x3f);
}

uint8_t ch2_read_reg2(void)
{
    return (env_volume << 4) | (env_direction << 3) | env_period;
}

void ch2_write_reg2(uint8_t val)
{
    env_volume = (val & 0xf0) >> 4;
    env_direction = (val & 0x8) >> 3;
    env_period = val & 0x7;
    env_count = env_period;
}

uint8_t ch2_read_reg3(void)
{
    return 0xff;
}

void ch2_write_reg3(uint8_t val)
{
    frequency = (frequency & 0x700) | val;
    timer = (2048 - frequency) * 4;
}

uint8_t ch2_read_reg4(void)
{
    return length_en | 0xbf;
}

uint8_t ch2_status(void)
{
    return status;
}

void ch2_write_reg4(uint8_t val)
{
    status = (val & 0x80) != 0;
    length_en = (val & 0x40) != 0;
    frequency = ((val & 0x7) << 8) | (frequency & 0xff);
    timer = (2048 - frequency) * 4;
    wave_ptr = 0;
    if (status && length_counter == 0) {
        length_counter = 64;
    }
}

void ch2_lengt_counter(void)
{
    if (length_en && length_counter > 0) {
        if (--length_counter == 0) {
            /* Disable output */
            status = 0;
        }
    }
}

void ch2_volume_envelope(void)
{
    if (env_period > 0 && --env_count <= 0) {
        env_count = env_period;
        if (env_direction) {
            if (env_volume < 0xf) {
                ++env_volume;
            }
        } else {
            if (env_volume > 0) {
                --env_volume;
            }
        }
    }
}

int16_t ch2_output(void)
{
    int16_t val = status & duty_table[wave_duty][wave_ptr];
    return val * env_volume;
}

void ch2_tick(unsigned int clock_step)
{
    timer -= clock_step;
    if (timer <= 0) {
        timer += (2048 - frequency) * 4;
        wave_ptr = (wave_ptr + 1) & 0x7;
    }
}
