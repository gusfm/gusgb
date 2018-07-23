#ifndef GPU_H
#define GPU_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define GB_SCREEN_WIDTH 160
#define GB_SCREEN_HEIGHT 144

typedef void (*render_callback_t)(void);

int gpu_init(SDL_Window *win, render_callback_t cb);
void gpu_finish(void);
void gpu_reset(void);

uint8_t gpu_read_lcdc(void);
uint8_t gpu_read_stat(void);
uint8_t gpu_read_scy(void);
uint8_t gpu_read_scx(void);
uint8_t gpu_read_ly(void);
uint8_t gpu_read_lyc(void);
uint8_t gpu_read_dma(void);
uint8_t gpu_read_bgp(void);
uint8_t gpu_read_obp0(void);
uint8_t gpu_read_obp1(void);
uint8_t gpu_read_wy(void);
uint8_t gpu_read_wx(void);
uint8_t gpu_read_vbk(void);
uint8_t gpu_read_bgpi(void);
uint8_t gpu_read_bgpd(void);
uint8_t gpu_read_obpi(void);
uint8_t gpu_read_obpd(void);

void gpu_write_lcdc(uint8_t val);
void gpu_write_stat(uint8_t val);
void gpu_write_scy(uint8_t val);
void gpu_write_scx(uint8_t val);
void gpu_write_lyc(uint8_t val);
void gpu_write_dma(uint8_t val);
void gpu_write_bgp(uint8_t val);
void gpu_write_obp0(uint8_t val);
void gpu_write_obp1(uint8_t val);
void gpu_write_wy(uint8_t val);
void gpu_write_wx(uint8_t val);
void gpu_write_vbk(uint8_t val);
void gpu_write_bgpi(uint8_t val);
void gpu_write_bgpd(uint8_t val);
void gpu_write_obpi(uint8_t val);
void gpu_write_obpd(uint8_t val);

uint8_t gpu_read_vram(uint16_t addr);
void gpu_write_vram(uint16_t addr, uint8_t val);
uint8_t gpu_read_oam(uint16_t addr);
void gpu_write_oam(uint16_t addr, uint8_t val);
void gpu_tick(unsigned int clock_step);
void gpu_render_framebuffer(void);
void gpu_change_speed(unsigned int speed);
void gpu_dump(void);

#endif /* GPU_H */
