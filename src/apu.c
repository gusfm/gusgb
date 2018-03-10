#include "apu.h"

uint8_t nr10, nr11, nr12, nr13, nr14;
uint8_t nr21, nr22, nr23, nr24;
uint8_t nr30, nr31, nr32, nr33, nr34;
uint8_t nr41, nr42, nr43, nr44;
uint8_t nr50, nr51, nr52;

void apu_sdl_cb(void *userdata, uint8_t *stream, int len)
{
    (void)userdata;
    (void)stream;
    (void)len;
}

void apu_reset(void)
{
    apu_write_nr10(0x80);
    apu_write_nr11(0xbf);
    apu_write_nr12(0xf3);
    // apu_write_nr13(0xff);
    apu_write_nr14(0xbf);
    apu_write_nr21(0x3f);
    apu_write_nr22(0x00);
    // apu_write_nr23(0xff);
    apu_write_nr24(0xbf);
    apu_write_nr30(0x7f);
    apu_write_nr31(0xff);
    apu_write_nr32(0x9f);
    apu_write_nr33(0xbf);
    // apu_write_nr34(0xff);
    apu_write_nr41(0xff);
    apu_write_nr42(0x00);
    apu_write_nr43(0x00);
    apu_write_nr44(0xbf);
    apu_write_nr50(0x77);
    apu_write_nr51(0xf3);
    apu_write_nr52(0xf1);
}

uint8_t apu_read_nr10(void)
{
    return nr10;
}

void apu_write_nr10(uint8_t val)
{
    nr10 = val;
}

uint8_t apu_read_nr11(void)
{
    return nr11;
}

void apu_write_nr11(uint8_t val)
{
    nr11 = val;
}

uint8_t apu_read_nr12(void)
{
    return nr12;
}

void apu_write_nr12(uint8_t val)
{
    nr12 = val;
}

uint8_t apu_read_nr13(void)
{
    return nr13;
}

void apu_write_nr13(uint8_t val)
{
    nr13 = val;
}

uint8_t apu_read_nr14(void)
{
    return nr14;
}

void apu_write_nr14(uint8_t val)
{
    nr14 = val;
}

uint8_t apu_read_nr21(void)
{
    return nr21;
}

void apu_write_nr21(uint8_t val)
{
    nr21 = val;
}

uint8_t apu_read_nr22(void)
{
    return nr22;
}

void apu_write_nr22(uint8_t val)
{
    nr22 = val;
}

uint8_t apu_read_nr23(void)
{
    return nr23;
}

void apu_write_nr23(uint8_t val)
{
    nr23 = val;
}

uint8_t apu_read_nr24(void)
{
    return nr24;
}

void apu_write_nr24(uint8_t val)
{
    nr24 = val;
}

uint8_t apu_read_nr30(void)
{
    return nr30;
}

void apu_write_nr30(uint8_t val)
{
    nr30 = val;
}

uint8_t apu_read_nr31(void)
{
    return nr31;
}

void apu_write_nr31(uint8_t val)
{
    nr31 = val;
}

uint8_t apu_read_nr32(void)
{
    return nr32;
}

void apu_write_nr32(uint8_t val)
{
    nr32 = val;
}

uint8_t apu_read_nr33(void)
{
    return nr33;
}

void apu_write_nr33(uint8_t val)
{
    nr33 = val;
}

uint8_t apu_read_nr34(void)
{
    return nr34;
}

void apu_write_nr34(uint8_t val)
{
    nr34 = val;
}

uint8_t apu_read_nr41(void)
{
    return nr41;
}

void apu_write_nr41(uint8_t val)
{
    nr41 = val;
}

uint8_t apu_read_nr42(void)
{
    return nr42;
}

void apu_write_nr42(uint8_t val)
{
    nr42 = val;
}

uint8_t apu_read_nr43(void)
{
    return nr43;
}

void apu_write_nr43(uint8_t val)
{
    nr43 = val;
}

uint8_t apu_read_nr44(void)
{
    return nr44;
}

void apu_write_nr44(uint8_t val)
{
    nr44 = val;
}

uint8_t apu_read_nr50(void)
{
    return nr50;
}

void apu_write_nr50(uint8_t val)
{
    nr50 = val;
}

uint8_t apu_read_nr51(void)
{
    return nr51;
}

void apu_write_nr51(uint8_t val)
{
    nr51 = val;
}

uint8_t apu_read_nr52(void)
{
    return nr52;
}

void apu_write_nr52(uint8_t val)
{
    nr51 = val;
}

uint8_t apu_read_wave(uint8_t addr)
{
    (void)addr;
    return 0xff;
}

void apu_write_wave(uint8_t addr, uint8_t val)
{
    (void)addr;
    (void)val;
}
