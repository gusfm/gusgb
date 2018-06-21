#include "apu.h"
#include <SDL2/SDL.h>
#include "../cartridge/cart.h"
#include "noise_ch.h"
#include "sqr_ch.h"
#include "timer.h"
#include "wave_ch.h"

/*** Registers ***/
/* 0xff24 (NR50): Vin sel and L/R Volume control (R/W) */
static uint8_t vin_sel_vol_ctrl;
/* 0xff25 (NR51): Selection of Sound output terminal (R/W) */
uint8_t ch_out_sel;
/* 0xff26 (NR52): Sound on/off */
uint8_t sound_enabled;

/*** Internal data ***/
static int16_t left_vol;  /* left volume: 0 - 32767 */
static int16_t right_vol; /* right volume: 0 - 32767 */
static int16_t out_buf[AUDIO_SAMPLE_SIZE];
apu_timer_t frame_sequencer = {0};
static apu_timer_t output_timer = {0};
static sqr_ch_t channel1;
static sqr_ch_t channel2;
static wave_ch_t channel3;
static noise_ch_t channel4;

static void apu_length_counter(void)
{
    if (sound_enabled) {
        sqr_ch_length_counter(&channel1);
        sqr_ch_length_counter(&channel2);
        wave_ch_length_counter(&channel3);
        noise_ch_length_counter(&channel4);
    }
}

static void apu_frame_sequencer_cb(unsigned int clock)
{
    switch (clock & 0x7) {
        case 0:
            apu_length_counter();
            break;
        case 1:
            break;
        case 2:
            sqr_ch_sweep(&channel1);
            apu_length_counter();
            break;
        case 3:
            break;
        case 4:
            apu_length_counter();
            break;
        case 5:
            break;
        case 6:
            sqr_ch_sweep(&channel1);
            apu_length_counter();
            break;
        case 7:
            sqr_ch_volume_envelope(&channel1);
            sqr_ch_volume_envelope(&channel2);
            noise_ch_volume_envelope(&channel4);
            break;
    }
}

static void apu_output_timer_cb(unsigned int clock)
{
    int left, right;
    int ch1 = sqr_ch_output(&channel1);
    int ch2 = sqr_ch_output(&channel2);
    int ch3 = wave_ch_output(&channel3);
    if (sound_enabled) {
        int lch1 = ch_out_sel & 0x1 ? ch1 : 0;
        int lch2 = ch_out_sel & 0x2 ? ch2 : 0;
        int lch3 = ch_out_sel & 0x4 ? ch3 : 0;
        int rch1 = ch_out_sel & 0x10 ? ch1 : 0;
        int rch2 = ch_out_sel & 0x20 ? ch2 : 0;
        int rch3 = ch_out_sel & 0x40 ? ch3 : 0;
        left = (lch1 + lch2 + lch3) * left_vol;
        right = (rch1 + rch2 + rch3) * right_vol;
    } else {
        left = 0;
        right = 0;
    }
    out_buf[clock++] = left;
    out_buf[clock++] = right;
    if (clock == AUDIO_SAMPLE_SIZE) {
        /* Delay while there are samples in the audio queue */
        while (SDL_GetQueuedAudioSize(1) >
               AUDIO_SAMPLE_SIZE * sizeof(int16_t)) {
            SDL_Delay(1);
        }
        SDL_QueueAudio(1, out_buf, AUDIO_SAMPLE_SIZE * sizeof(int16_t));
    }
}

void apu_reset(void)
{
    left_vol = 0;
    right_vol = 0;
    memset(out_buf, 0, sizeof(out_buf));
    apu_timer_init(&frame_sequencer, 8192, 1, 0x7, apu_frame_sequencer_cb);
    apu_timer_init(&output_timer, 87, 2, 0x3ff, apu_output_timer_cb);
    sqr_ch_reset(&channel1);
    sqr_ch_reset(&channel2);
    wave_ch_reset(&channel3);
    noise_ch_reset(&channel4);
    apu_write_nr10(0x80);
    apu_write_nr11(0xbf);
    apu_write_nr12(0xf3);
    apu_write_nr14(0xbf);
    apu_write_nr21(0x3f);
    apu_write_nr22(0x00);
    apu_write_nr24(0xbf);
    apu_write_nr30(0x7f);
    apu_write_nr31(0xff);
    apu_write_nr32(0x9f);
    apu_write_nr33(0xbf);
    apu_write_nr41(0xff);
    apu_write_nr42(0x00);
    apu_write_nr43(0x00);
    apu_write_nr44(0xbf);
    apu_write_nr50(0x77);
    apu_write_nr51(0xf3);
    apu_write_nr52(0xf1);
}

void apu_tick(unsigned int clock_step)
{
    apu_timer_tick(&frame_sequencer, clock_step);
    apu_timer_tick(&output_timer, clock_step);
    sqr_ch_tick(&channel1, clock_step);
    sqr_ch_tick(&channel2, clock_step);
    wave_ch_tick(&channel3, clock_step);
    noise_ch_tick(&channel4, clock_step);
}

uint8_t apu_read_nr10(void)
{
    return sqr_ch_read_reg0(&channel1);
}

void apu_write_nr10(uint8_t val)
{
    if (sound_enabled)
        sqr_ch_write_reg0(&channel1, val);
}

uint8_t apu_read_nr11(void)
{
    return sqr_ch_read_reg1(&channel1);
}

void apu_write_nr11(uint8_t val)
{
    if (sound_enabled || !cart_is_cgb())
        sqr_ch_write_reg1(&channel1, val);
}

uint8_t apu_read_nr12(void)
{
    return sqr_ch_read_reg2(&channel1);
}

void apu_write_nr12(uint8_t val)
{
    if (sound_enabled)
        sqr_ch_write_reg2(&channel1, val);
}

uint8_t apu_read_nr13(void)
{
    return sqr_ch_read_reg3(&channel1);
}

void apu_write_nr13(uint8_t val)
{
    if (sound_enabled)
        sqr_ch_write_reg3(&channel1, val);
}

uint8_t apu_read_nr14(void)
{
    return sqr_ch_read_reg4(&channel1);
}

void apu_write_nr14(uint8_t val)
{
    if (sound_enabled)
        sqr_ch_write_reg4(&channel1, val);
}

uint8_t apu_read_nr21(void)
{
    return sqr_ch_read_reg1(&channel2);
}

void apu_write_nr21(uint8_t val)
{
    if (sound_enabled || !cart_is_cgb())
        sqr_ch_write_reg1(&channel2, val);
}

uint8_t apu_read_nr22(void)
{
    return sqr_ch_read_reg2(&channel2);
}

void apu_write_nr22(uint8_t val)
{
    if (sound_enabled)
        sqr_ch_write_reg2(&channel2, val);
}

uint8_t apu_read_nr23(void)
{
    return sqr_ch_read_reg3(&channel2);
}

void apu_write_nr23(uint8_t val)
{
    if (sound_enabled)
        sqr_ch_write_reg3(&channel2, val);
}

uint8_t apu_read_nr24(void)
{
    return sqr_ch_read_reg4(&channel2);
}

void apu_write_nr24(uint8_t val)
{
    if (sound_enabled)
        sqr_ch_write_reg4(&channel2, val);
}

uint8_t apu_read_nr30(void)
{
    return wave_ch_read_reg0(&channel3);
}

void apu_write_nr30(uint8_t val)
{
    if (sound_enabled)
        wave_ch_write_reg0(&channel3, val);
}

uint8_t apu_read_nr31(void)
{
    return wave_ch_read_reg1(&channel3);
}

void apu_write_nr31(uint8_t val)
{
    if (sound_enabled || !cart_is_cgb())
        wave_ch_write_reg1(&channel3, val);
}

uint8_t apu_read_nr32(void)
{
    return wave_ch_read_reg2(&channel3);
}

void apu_write_nr32(uint8_t val)
{
    if (sound_enabled)
        wave_ch_write_reg2(&channel3, val);
}

uint8_t apu_read_nr33(void)
{
    return wave_ch_read_reg3(&channel3);
}

void apu_write_nr33(uint8_t val)
{
    if (sound_enabled)
        wave_ch_write_reg3(&channel3, val);
}

uint8_t apu_read_nr34(void)
{
    return wave_ch_read_reg4(&channel3);
}

void apu_write_nr34(uint8_t val)
{
    if (sound_enabled)
        wave_ch_write_reg4(&channel3, val);
}

uint8_t apu_read_nr41(void)
{
    return noise_ch_read_reg1(&channel4);
}

void apu_write_nr41(uint8_t val)
{
    if (sound_enabled || !cart_is_cgb())
        noise_ch_write_reg1(&channel4, val);
}

uint8_t apu_read_nr42(void)
{
    return noise_ch_read_reg2(&channel4);
}

void apu_write_nr42(uint8_t val)
{
    if (sound_enabled)
        noise_ch_write_reg2(&channel4, val);
}

uint8_t apu_read_nr43(void)
{
    return noise_ch_read_reg3(&channel4);
}

void apu_write_nr43(uint8_t val)
{
    if (sound_enabled)
        noise_ch_write_reg3(&channel4, val);
}

uint8_t apu_read_nr44(void)
{
    return noise_ch_read_reg4(&channel4);
}

void apu_write_nr44(uint8_t val)
{
    if (sound_enabled)
        noise_ch_write_reg4(&channel4, val);
}

uint8_t apu_read_nr50(void)
{
    return vin_sel_vol_ctrl;
}

void apu_write_nr50(uint8_t val)
{
    if (sound_enabled) {
        vin_sel_vol_ctrl = val;
        right_vol = ((vin_sel_vol_ctrl & 7) + 1) * 128;
        left_vol = (((vin_sel_vol_ctrl >> 4) & 7) + 1) * 128;
    }
}

uint8_t apu_read_nr51(void)
{
    return ch_out_sel;
}

void apu_write_nr51(uint8_t val)
{
    if (sound_enabled)
        ch_out_sel = val;
}

uint8_t apu_read_nr52(void)
{
    return sound_enabled | 0x70 | (noise_ch_status(&channel4) << 3) |
           (wave_ch_status(&channel3) << 2) | (sqr_ch_status(&channel2) << 1) |
           sqr_ch_status(&channel1);
}

void apu_write_nr52(uint8_t val)
{
    uint8_t old_enable = sound_enabled;
    uint8_t new_enable = 0x80 & val;
    if (old_enable) {
        if (!new_enable) {
            apu_write_nr10(0);
            apu_write_nr11(0);
            apu_write_nr12(0);
            apu_write_nr13(0);
            apu_write_nr14(0);
            apu_write_nr21(0);
            apu_write_nr22(0);
            apu_write_nr23(0);
            apu_write_nr24(0);
            apu_write_nr30(0);
            apu_write_nr31(0);
            apu_write_nr32(0);
            apu_write_nr33(0);
            apu_write_nr34(0);
            apu_write_nr41(0);
            apu_write_nr42(0);
            apu_write_nr43(0);
            apu_write_nr44(0);
            apu_write_nr50(0);
            apu_write_nr51(0);
        }
    } else {
        if (new_enable) {
            /* Reset frame sequencer when enabling master sound */
            frame_sequencer.in_clock = 0;
            frame_sequencer.out_clock = 0;
        }
    }
    sound_enabled = new_enable;
}

uint8_t apu_read_wave(int pos)
{
    return wave_ch_read_table(&channel3, pos);
}

void apu_write_wave(int pos, uint8_t val)
{
    wave_ch_write_table(&channel3, pos, val);
}
