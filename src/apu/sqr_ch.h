#ifndef SQR_CH
#define SQR_CH

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    unsigned int wave_duty;
    unsigned int wave_ptr;
    bool length_en;
    unsigned int length_counter;
    unsigned int frequency;
    bool status;
    int timer;
    unsigned int env_volume;
    unsigned int env_direction;
    unsigned int env_period;
    int env_count;
    unsigned int sweep_period;
    unsigned int sweep_negate;
    unsigned int sweep_shift;
    unsigned int sweep_enabled;
    unsigned int sweep_frequency;
} sqr_ch_t;

void sqr_ch_reset(sqr_ch_t *c);
void sqr_ch_tick(sqr_ch_t *c, unsigned int clock_step);
uint8_t sqr_ch_status(sqr_ch_t *c);
int16_t sqr_ch_output(sqr_ch_t *c);

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
