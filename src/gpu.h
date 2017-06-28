#ifndef __GPU_H__
#define __GPU_H__

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    GPU_MODE_HBLANK = 0,
    GPU_MODE_VBLANK = 1,
    GPU_MODE_OAM = 2,
    GPU_MODE_VRAM = 3,
} gpu_mode_e;

typedef struct {
    uint8_t r, g, b;
} rgb_t;

typedef void (*render_callback_t)(void);

typedef struct {
    /* 0xff40 - LCDC - LCD Control (R/W) */
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
    /* 0xff41 - STAT - LCDC Status (R/W) */
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
    /* 0xff42 - SCY - Scroll Y (R/W) */
    uint8_t scroll_y;
    /* 0xff43 - SCX - Scroll X (R/W) */
    uint8_t scroll_x;
    /* 0xff44 - LY - LCDC Y-Coordinate (R) */
    uint8_t scanline;
    /* 0xff45 - LYC - LY Compare (R/W) */
    uint8_t raster;
    /* 0xff46 - DMA - DMA Transfer and Start Address (W) */
    uint8_t dma;
    /* 0xff47 - BGP - BG Palette Data (R/W) - Non CGB Mode Only */
    uint8_t bgp;
    /* 0xff48 - OBP0 - Object Palette 0 Data (R/W) - Non CGB Mode Only */
    uint8_t obp0;
    /* 0xff49 - OBP1 - Object Palette 1 Data (R/W) - Non CGB Mode Only */
    uint8_t obp1;
    /* 0xff4a - WY - Window Y Position (R/W) */
    uint8_t window_y;
    /* 0xff4b - WX - Window X Position minus 7 (R/W) */
    uint8_t window_x;
    /* FF4F - VBK - CGB Mode Only - VRAM Bank */
    uint8_t vram_bank;
    /* FF68 - BCPS/BGPI - CGB Mode Only - Background Palette Index */
    uint8_t cgb_bg_pal_idx;
    /* FF69 - BCPD/BGPD - CGB Mode Only - Background Palette Data */
    uint8_t cgb_bg_pal_data[8 * 8];
    /* FF6A - OCPS/OBPI - CGB Mode Only - Sprite Palette Index */
    uint8_t cgb_sprite_pal_idx;
    /* FF6B - OCPD/OBPD - CGB Mode Only - Sprite Palette Data */
    uint8_t cgb_sprite_pal_data[8 * 8];
    uint32_t modeclock;
    uint8_t vram[2][0x2000]; /* Video RAM. */
    uint8_t oam[0xa0];    /* Sprite info. */
    rgb_t framebuffer[160 * 144];
    rgb_t bg_palette[8 * 4];
    rgb_t sprite_palette[8 * 4];
    unsigned int speed;
} gpu_t;

typedef struct {
    GLFWwindow *window;
    render_callback_t callback;
    float zoom;
    bool gl_enabled;
} gpu_gl_t;

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

void gpu_init(float zoom);
void gpu_reset(void);
gpu_t *gpu_get_instance(void);
void gpu_set_callback(render_callback_t cb);
void gpu_set_glfw_window(GLFWwindow *window);
void gpu_gl_enable(void);
void gpu_gl_disable(void);
rgb_t *gpu_get_framebuffer(void);
uint8_t gpu_read_byte(uint16_t addr);
void gpu_write_byte(uint16_t addr, uint8_t val);
uint8_t gpu_read_vram(uint16_t addr);
void gpu_write_vram(uint16_t addr, uint8_t val);
uint8_t gpu_read_oam(uint16_t addr);
void gpu_write_oam(uint16_t addr, uint8_t val);
void gpu_step(uint32_t cpu_tick);
void gpu_render_framebuffer(void);
void gpu_change_speed(unsigned int speed);
void gpu_dump(void);

#endif /* __GPU_H__ */
