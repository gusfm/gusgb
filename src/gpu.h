#ifndef __GPU_H__
#define __GPU_H__

#include <GLFW/glfw3.h>
#include <stdint.h>

#define GPU_CONTROL_BGENABLE (1 << 0)
#define GPU_CONTROL_SPRITEENABLE (1 << 1)
#define GPU_CONTROL_SPRITEVDOUBLE (1 << 2)
#define GPU_CONTROL_TILEMAP (1 << 3)
#define GPU_CONTROL_TILESET (1 << 4)
#define GPU_CONTROL_WINDOWENABLE (1 << 5)
#define GPU_CONTROL_WINDOWTILEMAP (1 << 6)
#define GPU_CONTROL_DISPLAYENABLE (1 << 7)

#define LCD_INT_HBLANK 0x08 // HBLANK interrupt
#define LCD_INT_VBLANK 0x10 // VBLANK interrupt
#define LCD_INT_OAM    0x20 // OAM interrupt
#define LCD_INT_LY_LYC 0x40 // LY/LYC coincidence interrupt

typedef enum {
    GPU_MODE_HBLANK = 0,
    GPU_MODE_VBLANK = 1,
    GPU_MODE_OAM = 2,
    GPU_MODE_VRAM = 3,
} gpu_mode_e;

typedef struct {
    uint8_t r, g, b;
} rgb_t;

typedef struct {
    uint8_t reg[0x100];
    union {
        struct {
            uint8_t bg_on : 1;
            uint8_t sprites_on : 1;
            uint8_t sprites_size : 1;
            uint8_t bg_tile_map : 1;
            uint8_t bg_tile_set : 1;
            uint8_t window_on : 1;
            uint8_t window_tile_map : 1;
            uint8_t display_on : 1;
        };
        uint8_t control;
    };
    uint8_t lcd_status;
    uint8_t scroll_x;
    uint8_t scroll_y;
    uint8_t window_x;
    uint8_t window_y;
    uint8_t scanline;
    uint8_t raster;
    uint32_t modeclock;
    uint8_t tiles[384][8][8];
    gpu_mode_e linemode;
    uint8_t vram[0x2000]; /* Video RAM. */
    uint8_t oam[0xa0];   /* Sprite info. */
    rgb_t framebuffer[160 * 144];
    rgb_t bg_palette[4];
    rgb_t sprite_palette[2][4];
    GLFWwindow *window;
    float zoom;
} gpu_t;

typedef struct {
    uint8_t y;    /* Y-coordinate minus 16. */
    uint8_t x;    /* X-coordinate minus 8. */
    uint8_t tile; /* Tile number. */
    union {
        uint8_t options;
        struct {
            uint8_t unused : 4;
            uint8_t palette : 1;
            uint8_t xflip : 1;
            uint8_t yflip : 1;
            uint8_t priority : 1;
        };
    };
} sprite_t;

void gpu_init(float zoom);
void gpu_set_glfw_window(GLFWwindow *window);
uint8_t gpu_read_byte(uint16_t addr);
void gpu_write_byte(uint16_t addr, uint8_t val);
uint8_t gpu_read_vram(uint16_t addr);
void gpu_write_vram(uint16_t addr, uint8_t val);
uint8_t gpu_read_oam(uint16_t addr);
void gpu_write_oam(uint16_t addr, uint8_t val);
void gpu_step(uint32_t cpu_tick);

#endif /* __GPU_H__ */
