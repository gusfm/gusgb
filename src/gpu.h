#ifndef GPU_H
#define GPU_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include "color.h"

#define GB_SCREEN_WIDTH 160
#define GB_SCREEN_HEIGHT 144

typedef struct {
    uint8_t data_h;
    uint8_t data_l;
} tile_line_t;

typedef struct {
    union {
        uint8_t attributes;
        struct {
            uint8_t pal_number : 3; /* BGP0-7 */
            uint8_t vram_bank : 1;  /* (0=Bank 0, 1=Bank 1) */
            uint8_t unused : 1;
            uint8_t hflip : 1;    /* (0=Normal, 1=Mirror horizontally) */
            uint8_t vflip : 1;    /* (0=Normal, 1=Mirror vertically) */
            uint8_t priority : 1; /* (0=Use OAM priority bit, 1=BG Priority) */
        };
    };
} bg_attr_t;

typedef void (*render_callback_t)(void);

int gpu_init(SDL_Renderer *ren, SDL_Texture *tex, render_callback_t cb);
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

extern int gpu_get_tile_id(int mapoffs);
extern bg_attr_t gpu_get_tile_attributes(int tile_id);
extern tile_line_t gpu_get_tile_line(bg_attr_t attr, int tile_id, int y);
extern int gpu_get_tile_color(tile_line_t tile_line, int tile_x, bool hflip);

#ifdef DEBUGGER
color_t *gpu_get_bg_palette(void);
color_t *gpu_get_sprite_palette(void);
#endif /* DEBUGGER */

#endif /* GPU_H */
