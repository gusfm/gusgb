#ifndef APU_H
#define APU_H

#include <stdint.h>

#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_SAMPLE_SIZE 1024

void apu_reset(void);
void apu_tick(unsigned int clock_step);
void apu_change_speed(unsigned int new_speed);

uint8_t apu_read_nr10(void);
uint8_t apu_read_nr11(void);
uint8_t apu_read_nr12(void);
uint8_t apu_read_nr13(void);
uint8_t apu_read_nr14(void);
uint8_t apu_read_nr21(void);
uint8_t apu_read_nr22(void);
uint8_t apu_read_nr23(void);
uint8_t apu_read_nr24(void);
uint8_t apu_read_nr30(void);
uint8_t apu_read_nr31(void);
uint8_t apu_read_nr32(void);
uint8_t apu_read_nr33(void);
uint8_t apu_read_nr34(void);
uint8_t apu_read_nr41(void);
uint8_t apu_read_nr42(void);
uint8_t apu_read_nr43(void);
uint8_t apu_read_nr44(void);
uint8_t apu_read_nr50(void);
uint8_t apu_read_nr51(void);
uint8_t apu_read_nr52(void);
uint8_t apu_read_wave(int pos);

void apu_write_nr10(uint8_t val);
void apu_write_nr11(uint8_t val);
void apu_write_nr12(uint8_t val);
void apu_write_nr13(uint8_t val);
void apu_write_nr14(uint8_t val);
void apu_write_nr21(uint8_t val);
void apu_write_nr22(uint8_t val);
void apu_write_nr23(uint8_t val);
void apu_write_nr24(uint8_t val);
void apu_write_nr30(uint8_t val);
void apu_write_nr31(uint8_t val);
void apu_write_nr32(uint8_t val);
void apu_write_nr33(uint8_t val);
void apu_write_nr34(uint8_t val);
void apu_write_nr41(uint8_t val);
void apu_write_nr42(uint8_t val);
void apu_write_nr43(uint8_t val);
void apu_write_nr44(uint8_t val);
void apu_write_nr50(uint8_t val);
void apu_write_nr51(uint8_t val);
void apu_write_nr52(uint8_t val);
void apu_write_wave(int pos, uint8_t val);

#endif /* APU_H */
