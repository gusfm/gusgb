#ifndef NOISE_CH
#define NOISE_CH

#include <stdbool.h>
#include <stdint.h>
#include "length_counter.h"
#include "volume_envelope.h"

typedef struct {
    bool enabled;
    uint8_t shift_clock;
    uint8_t counter_step;
    uint8_t div_ratio;
    struct length_counter length;
    struct volume_envelope env;
} noise_ch_t;

void noise_ch_reset(noise_ch_t *c);
void noise_ch_tick(noise_ch_t *c);
bool noise_ch_status(noise_ch_t *c);
int noise_ch_output(noise_ch_t *c);

void noise_ch_length_counter(noise_ch_t *c);
void noise_ch_volume_envelope(noise_ch_t *c);

uint8_t noise_ch_read_reg1(noise_ch_t *c);
uint8_t noise_ch_read_reg2(noise_ch_t *c);
uint8_t noise_ch_read_reg3(noise_ch_t *c);
uint8_t noise_ch_read_reg4(noise_ch_t *c);

void noise_ch_write_reg1(noise_ch_t *c, uint8_t val);
void noise_ch_write_reg2(noise_ch_t *c, uint8_t val);
void noise_ch_write_reg3(noise_ch_t *c, uint8_t val);
void noise_ch_write_reg4(noise_ch_t *c, uint8_t val);

#endif /* NOISE_CH */
