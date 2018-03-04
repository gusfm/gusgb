#ifndef GPU_H
#define GPU_H

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define GB_SCREEN_WIDTH 160
#define GB_SCREEN_HEIGHT 144

typedef enum {
    GPU_MODE_HBLANK = 0,
    GPU_MODE_VBLANK = 1,
    GPU_MODE_OAM = 2,
    GPU_MODE_VRAM = 3,
} gpu_mode_e;

typedef void (*render_callback_t)(void);

typedef struct {
#if (SDL_BYTE_ORDER == SDL_BIG_ENDIAN)
    uint8_t a, r, g, b;
#else
    uint8_t b, g, r, a;
#endif
} color_t;

typedef struct {
    /* 0xff40 (LCDC): LCD Control (R/W) */
    union {
        uint8_t lcd_control;
        struct {
            uint8_t bg_display : 1;
            uint8_t obj_enable : 1;
            uint8_t obj_size : 1;
            uint8_t bg_tile_map : 1;
            uint8_t bg_tile_set : 1;
            uint8_t window_enable : 1;
            uint8_t window_tile_map : 1;
            uint8_t lcd_enable : 1;
        };
    };
    /* 0xff41 (STAT): LCDC Status (R/W) */
    union {
        uint8_t lcd_status;
        struct {
            uint8_t mode_flag : 2;
            uint8_t coincidence_flag : 1;
            uint8_t hblank_int : 1;
            uint8_t vblank_int : 1;
            uint8_t oam_int : 1;
            uint8_t coincidence_int : 1;
            uint8_t unused1 : 1;
        };
    };
    /* 0xff42 (SCY): Scroll Y (R/W) */
    uint8_t scroll_y;
    /* 0xff43 (SCX): Scroll X (R/W) */
    uint8_t scroll_x;
    /* 0xff44 (LY): LCDC Y-Coordinate (R) */
    uint8_t scanline;
    /* 0xff45 (LYC): LY Compare (R/W) */
    uint8_t lyc;
    /* 0xff46 (DMA): DMA Transfer and Start Address (W) */
    uint8_t dma;
    /* 0xff47 (BGP): BG Palette Data (R/W) - Non CGB */
    uint8_t bgp;
    /* 0xff48 (OBP0): Object Palette 0 Data (R/W) - Non CGB */
    uint8_t obp0;
    /* 0xff49 (OBP1): Object Palette 1 Data (R/W) - Non CGB */
    uint8_t obp1;
    /* 0xff4a (WY): Window Y Position (R/W) */
    uint8_t window_y;
    /* 0xff4b (WX): Window X Position minus 7 (R/W) */
    uint8_t window_x;
    /* 0xff4f (VBK): VRAM Bank - CGB only */
    uint8_t vram_bank;
    /* 0xff68 (BGPI): Background Palette Index - CGB only */
    uint8_t cgb_bg_pal_idx;
    /* 0xff69 (BGPD): Background Palette Data - CGB only */
    uint8_t cgb_bg_pal_data[8 * 8];
    /* 0xff6a (OBPI): Sprite Palette Index - CGB only */
    uint8_t cgb_sprite_pal_idx;
    /* 0xff6b (OBPD): Sprite Palette Data - CGB only*/
    uint8_t cgb_sprite_pal_data[8 * 8];
    uint32_t modeclock;
    uint8_t vram[2][0x2000]; /* Video RAM. */
    uint8_t oam[0xa0];       /* Sprite info. */
    color_t framebuffer[GB_SCREEN_WIDTH * GB_SCREEN_HEIGHT];
    color_t bg_palette[8 * 4];
    color_t sprite_palette[8 * 4];
    unsigned int speed;
} gpu_t;

typedef struct {
    uint8_t y;    /* Y-coordinate minus 16. */
    uint8_t x;    /* X-coordinate minus 8. */
    uint8_t tile; /* Tile number. */
    union {
        uint8_t options;
        struct {
            uint8_t cgb_palette : 3;
            uint8_t cgb_vram_bank : 1;
            uint8_t palette : 1;
            uint8_t xflip : 1;
            uint8_t yflip : 1;
            uint8_t priority : 1;
        };
    };
} sprite_t;

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
} cgb_bg_attr_t;

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
void gpu_step(uint32_t cpu_tick);
void gpu_render_framebuffer(void);
void gpu_change_speed(unsigned int speed);
void gpu_dump(void);

#endif /* GPU_H */
