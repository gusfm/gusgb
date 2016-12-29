#include "gpu.h"
#include <stdio.h>
#include <string.h>
#include "interrupt.h"

gpu_t g_gpu;

const rgb_t g_palette[4] = {
    {255, 255, 255},  // off
    {192, 192, 192},  // 33% on
    {96, 96, 96},     // 66% on
    {0, 0, 0},        // on
};

void gpu_init(uint8_t *vram, uint8_t *oam)
{
    memset(&g_gpu, 0, sizeof(g_gpu));
    g_gpu.vram = vram;
    g_gpu.oam = oam;
    /* Init background palette. */
    g_gpu.bg_palette[0] = g_palette[0];
    g_gpu.bg_palette[1] = g_palette[1];
    g_gpu.bg_palette[2] = g_palette[2];
    g_gpu.bg_palette[3] = g_palette[3];
    /* Init sprite palette. */
    g_gpu.sprite_palette[0][0] = g_palette[0];
    g_gpu.sprite_palette[0][1] = g_palette[1];
    g_gpu.sprite_palette[0][2] = g_palette[2];
    g_gpu.sprite_palette[0][3] = g_palette[3];
    /* Init sprite palette. */
    g_gpu.sprite_palette[1][0] = g_palette[0];
    g_gpu.sprite_palette[1][1] = g_palette[1];
    g_gpu.sprite_palette[1][2] = g_palette[2];
    g_gpu.sprite_palette[1][3] = g_palette[3];
}

void gpu_set_glfw_window(GLFWwindow *window)
{
    g_gpu.window = window;
}

uint8_t gpu_get_lcd_control(void)
{
    return g_gpu.control;
}

void gpu_set_lcd_control(uint8_t value)
{
    g_gpu.control = value;
}

uint8_t gpu_get_scroll_y(void)
{
    return g_gpu.scroll_y;
}

void gpu_set_scroll_y(uint8_t value)
{
    g_gpu.scroll_y = value;
}

uint8_t gpu_get_scroll_x(void)
{
    return g_gpu.scroll_x;
}

void gpu_set_scroll_x(uint8_t value)
{
    g_gpu.scroll_x = value;
}

uint8_t gpu_get_scanline(void)
{
    return g_gpu.scanline;
}

static void gpu_hblank(void)
{
    g_gpu.scanline++;
}

void gpu_set_bg_palette(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        g_gpu.bg_palette[i] = g_palette[(value >> (i * 2)) & 3];
    }
}

void gpu_set_sprite_palette0(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        g_gpu.sprite_palette[0][i] = g_palette[(value >> (i * 2)) & 3];
    }
}

void gpu_set_sprite_palette1(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        g_gpu.sprite_palette[1][i] = g_palette[(value >> (i * 2)) & 3];
    }
}

static void gpu_render_sprites(uint8_t *scanline_row)
{
    // if sprites enabled
    for (int i = 0; i < 40; i++) {
        sprite_t sprite = ((sprite_t *)g_gpu.oam)[i];

        int sx = sprite.x - 8;
        int sy = sprite.y - 16;

        if (sy <= g_gpu.scanline && (sy + 8) > g_gpu.scanline) {
            rgb_t *pal = g_gpu.sprite_palette[sprite.palette];

            int pixel_offset;
            pixel_offset = g_gpu.scanline * 160 + sx;

            uint8_t tile_row;
            if (sprite.vFlip)
                tile_row = (uint8_t)(7 - (g_gpu.scanline - sy));
            else
                tile_row = (uint8_t)(g_gpu.scanline - sy);

            for (int x = 0; x < 8; x++) {
                if (sx + x >= 0 && sx + x < 160 &&
                    (~sprite.priority || !scanline_row[sx + x])) {
                    uint8_t colour;

                    if (sprite.hFlip)
                        colour = g_gpu.tiles[sprite.tile][tile_row][7 - x];
                    else
                        colour = g_gpu.tiles[sprite.tile][tile_row][x];

                    if (colour) {
                        g_gpu.framebuffer[pixel_offset].r = pal[colour].r;
                        g_gpu.framebuffer[pixel_offset].g = pal[colour].g;
                        g_gpu.framebuffer[pixel_offset].b = pal[colour].b;
                    }

                    pixel_offset++;
                }
            }
        }
    }
}

static void gpu_render_scanline(void)
{
    int mapoffs = (g_gpu.control & GPU_CONTROL_TILEMAP) ? 0x1c00 : 0x1800;
    mapoffs += (((g_gpu.scanline + g_gpu.scroll_y) & 255) >> 3) << 5;

    int lineoffs = (g_gpu.scroll_x >> 3);

    int x = g_gpu.scroll_x & 7;
    int y = (g_gpu.scanline + g_gpu.scroll_y) & 7;

    int pixeloffs;
    pixeloffs = g_gpu.scanline * 160;

    uint16_t tile = (uint16_t)g_gpu.vram[mapoffs + lineoffs];
    // if ((g_gpu.control & GPU_CONTROL_TILESET) && tile < 128)
    //    tile += 256;

    uint8_t scanline_row[160];

    printf("%s:%d: scanline\n", __func__, __LINE__);
    // if bg enabled
    for (int i = 0; i < 160; i++) {
        uint8_t colour = g_gpu.tiles[tile][y][x];

        scanline_row[i] = colour;

        g_gpu.framebuffer[pixeloffs].r = g_gpu.bg_palette[colour].r;
        g_gpu.framebuffer[pixeloffs].g = g_gpu.bg_palette[colour].g;
        g_gpu.framebuffer[pixeloffs].b = g_gpu.bg_palette[colour].b;
        pixeloffs++;

        x++;
        if (x == 8) {
            x = 0;
            lineoffs = (lineoffs + 1) & 31;
            tile = g_gpu.vram[mapoffs + lineoffs];
            // if ((g_gpu.control & GPU_CONTROL_TILESET) && tile < 128)
            // tile += 256;
        }
    }

    gpu_render_sprites(scanline_row);
}

void gpu_step(uint32_t cpu_tick)
{
    g_gpu.tick += cpu_tick - g_gpu.last_tick;
    g_gpu.last_tick = cpu_tick;

    switch (g_gpu.gpu_mode) {
        case GPU_MODE_HBLANK:
            if (g_gpu.tick >= 204) {
                gpu_hblank();
                if (g_gpu.scanline == 143) {
                    if (interrupt_is_enable(INTERRUPTS_VBLANK)) {
                        interrupt_set_flag_bit(INTERRUPTS_VBLANK);
                    }
                    g_gpu.gpu_mode = GPU_MODE_VBLANK;
                } else {
                    g_gpu.gpu_mode = GPU_MODE_OAM;
                }
                g_gpu.tick -= 204;
            }
            break;

        case GPU_MODE_VBLANK:
            if (g_gpu.tick >= 456) {
                g_gpu.scanline++;
                if (g_gpu.scanline > 153) {
                    g_gpu.scanline = 0;
                    g_gpu.gpu_mode = GPU_MODE_OAM;
                }
                g_gpu.tick -= 456;
            }
            break;

        case GPU_MODE_OAM:
            if (g_gpu.tick >= 80) {
                g_gpu.gpu_mode = GPU_MODE_VRAM;
                g_gpu.tick -= 80;
            }
            break;

        case GPU_MODE_VRAM:
            if (g_gpu.tick >= 172) {
                g_gpu.gpu_mode = GPU_MODE_HBLANK;
                gpu_render_scanline();
                g_gpu.tick -= 172;
            }
            break;
    }
}

void gpu_update_tile(uint16_t addr)
{
    uint16_t vram_index = addr & 0x1fff;
    uint16_t tile = (vram_index >> 4) & 511;
    uint16_t y = (vram_index >> 1) & 7;
    for (uint8_t x = 0; x < 8; ++x) {
        uint8_t bit_index = (uint8_t)(1 << (7 - x));
        g_gpu.tiles[tile][y][x] =
            (uint8_t)(((g_gpu.vram[vram_index] & bit_index) ? 1u : 0u) +
                      ((g_gpu.vram[vram_index + 1] & bit_index) ? 2u : 0u));
        //printf("%s:%d: tile=%hu, y=%hu, x=%hhu\n", __func__, __LINE__, tile, y, x);
    }
}

void gpu_render_framebuffer(void)
{
    // Should render to a texture instead of obsolete glDrawPixels

    // framebuffer[x + y * 160].r = 255;
    // framebuffer[x + y * 160].g = 255;
    // framebuffer[x + y * 160].b = 255;

    printf("%s:%d: render\n", __func__, __LINE__);
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2f(-1, 1);
    glPixelZoom(1, -1);
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, g_gpu.framebuffer);
    glfwSwapBuffers(g_gpu.window);
    glfwPollEvents();
}
