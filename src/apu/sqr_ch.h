#ifndef SQR_CH
#define SQR_CH

#include <stdbool.h>
#include <stdint.h>
#include "length_counter.h"

struct volume_envelope {
    unsigned int volume;
    unsigned int direction;
    unsigned int period;
    int count;
};

struct sweep {
    unsigned int period;
    unsigned int period_tmp;
    unsigned int negate;
    unsigned int shift;
    unsigned int enabled;
    unsigned int frequency;
    bool neg_calc;
};

typedef struct {
    unsigned int wave_duty;
    unsigned int wave_ptr;
    unsigned int frequency;
    bool enabled;
    int timer;
    struct length_counter length;
    struct volume_envelope env;
    struct sweep sweep;
} sqr_ch_t;

void sqr_ch_reset(sqr_ch_t *c);
void sqr_ch_tick(sqr_ch_t *c, unsigned int clock_step);
uint8_t sqr_ch_status(sqr_ch_t *c);
int sqr_ch_output(sqr_ch_t *c);

void sqr_ch_sweep(sqr_ch_t *c);
void sqr_ch_length_counter(sqr_ch_t *c);
void sqr_ch_volume_envelope(sqr_ch_t *c);

uint8_t sqr_ch_read_reg0(sqr_ch_t *c);
uint8_t sqr_ch_read_reg1(sqr_ch_t *c);
uint8_t sqr_ch_read_reg2(sqr_ch_t *c);
uint8_t sqr_ch_read_reg3(sqr_ch_t *c);
uint8_t sqr_ch_read_reg4(sqr_ch_t *c);

void sqr_ch_write_reg0(sqr_ch_t *c, uint8_t val);
void sqr_ch_write_reg1(sqr_ch_t *c, uint8_t val);
void sqr_ch_write_reg2(sqr_ch_t *c, uint8_t val);
void sqr_ch_write_reg3(sqr_ch_t *c, uint8_t val);
void sqr_ch_write_reg4(sqr_ch_t *c, uint8_t val);

#endif /* SQR_CH */
