#ifndef WAVE_CH
#define WAVE_CH

#include <stdbool.h>
#include <stdint.h>
#include "length_counter.h"

typedef struct {
    unsigned int frequency;
    int timer;
    int position;
    bool enabled;
    bool status;
    uint8_t table[16];
    uint8_t sound_length;
    uint8_t volume;
    uint8_t out_volume;
    struct length_counter length;
} wave_ch_t;

void wave_ch_reset(wave_ch_t *c);
void wave_ch_tick(wave_ch_t *c, unsigned int clock_step);
uint8_t wave_ch_status(wave_ch_t *c);
uint8_t wave_ch_output(wave_ch_t *c);

void wave_ch_length_counter(wave_ch_t *c);

uint8_t wave_ch_read_reg0(wave_ch_t *c);
uint8_t wave_ch_read_reg1(wave_ch_t *c);
uint8_t wave_ch_read_reg2(wave_ch_t *c);
uint8_t wave_ch_read_reg3(wave_ch_t *c);
uint8_t wave_ch_read_reg4(wave_ch_t *c);
uint8_t wave_ch_read_table(wave_ch_t *c, int pos);

void wave_ch_write_reg0(wave_ch_t *c, uint8_t val);
void wave_ch_write_reg1(wave_ch_t *c, uint8_t val);
void wave_ch_write_reg2(wave_ch_t *c, uint8_t val);
void wave_ch_write_reg3(wave_ch_t *c, uint8_t val);
void wave_ch_write_reg4(wave_ch_t *c, uint8_t val);
void wave_ch_write_table(wave_ch_t *c, int pos, uint8_t val);

#endif /* WAVE_CH */
