#ifndef CH2_H
#define CH2_H

#include <stdint.h>

void ch2_reset(void);
void ch2_tick(unsigned int clock_step);
uint8_t ch2_status(void);

void ch2_lengt_counter(void);
void ch2_volume_envelope(void);
void ch2_output(int16_t *out_buf);

uint8_t ch2_read_reg1(void);
uint8_t ch2_read_reg2(void);
uint8_t ch2_read_reg3(void);
uint8_t ch2_read_reg4(void);

void ch2_write_reg1(uint8_t val);
void ch2_write_reg2(uint8_t val);
void ch2_write_reg3(uint8_t val);
void ch2_write_reg4(uint8_t val);

#endif /* CH2_H */
