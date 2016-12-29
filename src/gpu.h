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
    uint8_t control;
    uint8_t scroll_x;
    uint8_t scroll_y;
    uint8_t scanline;
    uint32_t tick;
    uint32_t last_tick;
    uint8_t tiles[384][8][8];
    gpu_mode_e gpu_mode;
    uint8_t *vram;
    uint8_t *oam;
    rgb_t framebuffer[160 * 144];
    rgb_t bg_palette[4];
    rgb_t sprite_palette[2][4];
    GLFWwindow *window;
} gpu_t;

typedef struct {
    uint8_t y;
    uint8_t x;
    uint8_t tile;
    uint8_t priority : 1;
    uint8_t vFlip : 1;
    uint8_t hFlip : 1;
    uint8_t palette : 1;
} sprite_t;

void gpu_init(uint8_t *vram, uint8_t *oam);
void gpu_set_glfw_window(GLFWwindow *window);
uint8_t gpu_get_lcd_control(void);
void gpu_set_lcd_control(uint8_t value);
uint8_t gpu_get_scroll_y(void);
void gpu_set_scroll_y(uint8_t value);
uint8_t gpu_get_scroll_x(void);
void gpu_set_scroll_x(uint8_t value);
uint8_t gpu_get_scanline(void);
void gpu_step(uint32_t cpu_tick);
void gpu_update_tile(uint16_t addr);
void gpu_render_framebuffer(void);

#endif /* __GPU_H__ */
