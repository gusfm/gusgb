#include "gpu.h"
#include <stdio.h>
#include <string.h>
#include "interrupt.h"
#include "mmu.h"

gpu_t GPU;

const rgb_t g_palette[4] = {
    {255, 255, 255},  // off
    {192, 192, 192},  // 33% on
    {96, 96, 96},     // 66% on
    {0, 0, 0},        // on
};

void gpu_init(float zoom)
{
    memset(&GPU, 0, sizeof(GPU));
    GPU.linemode = GPU_MODE_OAM;
    /* Init background palette. */
    GPU.bg_palette[0] = g_palette[0];
    GPU.bg_palette[1] = g_palette[1];
    GPU.bg_palette[2] = g_palette[2];
    GPU.bg_palette[3] = g_palette[3];
    /* Init sprite palette. */
    GPU.sprite_palette[0][0] = g_palette[0];
    GPU.sprite_palette[0][1] = g_palette[1];
    GPU.sprite_palette[0][2] = g_palette[2];
    GPU.sprite_palette[0][3] = g_palette[3];
    /* Init sprite palette. */
    GPU.sprite_palette[1][0] = g_palette[0];
    GPU.sprite_palette[1][1] = g_palette[1];
    GPU.sprite_palette[1][2] = g_palette[2];
    GPU.sprite_palette[1][3] = g_palette[3];
    GPU.zoom = zoom;
}

void gpu_set_glfw_window(GLFWwindow *window)
{
    GPU.window = window;
}

uint8_t gpu_read_vram(uint16_t addr)
{
    return GPU.vram[addr & 0x1fff];
}

void gpu_write_vram(uint16_t addr, uint8_t val)
{
    GPU.vram[addr & 0x1fff] = val;
}

uint8_t gpu_read_oam(uint16_t addr)
{
    return GPU.oam[addr & 0xff];
}

void gpu_write_oam(uint16_t addr, uint8_t val)
{
    GPU.oam[addr & 0xff] = val;
}

static void gpu_set_bg_palette(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        GPU.bg_palette[i] = g_palette[(value >> (i * 2)) & 3];
    }
}

static void gpu_set_sprite_palette0(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        GPU.sprite_palette[0][i] = g_palette[(value >> (i * 2)) & 3];
    }
}

static void gpu_set_sprite_palette1(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        GPU.sprite_palette[1][i] = g_palette[(value >> (i * 2)) & 3];
    }
}

uint8_t gpu_read_byte(uint16_t addr)
{
    switch (addr) {
        case 0xff40:
            return GPU.control;
        case 0xff41:
            return (uint8_t)((GPU.scanline == GPU.raster ? 4 : 0) |
                             GPU.linemode);
        case 0xff42:
            return GPU.scroll_y;
        case 0xff43:
            return GPU.scroll_x;
        case 0xff44:
            return GPU.scanline;
        case 0xff45:
            return GPU.raster;
        case 0xff4a:
            return GPU.window_y;
        case 0xff4b:
            return GPU.window_x;
        default:
            printf("gpu_read_byte: not implemented: 0x%04x\n", addr);
            return GPU.reg[addr - 0xff40];
    }
}

void gpu_write_byte(uint16_t addr, uint8_t val)
{
    GPU.reg[addr - 0xff40] = val;
    switch (addr) {
        case 0xff40:
            GPU.control = val;
            break;
        case 0xff42:
            GPU.scroll_y = val;
            break;
        case 0xff43:
            GPU.scroll_x = val;
            break;
        case 0xff45:
            GPU.raster = val;
            break;
        case 0xff46:
            /* OAM DMA. */
            for (int i = 0; i < 160; i++) {
                uint16_t dma_addr = (uint16_t)((val << 8) + i);
                uint8_t v = mmu_read_byte(dma_addr);
                GPU.oam[i] = v;
            }
            break;
        case 0xff47:
            /* BG palette mapping. */
            gpu_set_bg_palette(val);
            break;
        case 0xff48:
            /* OBJ0 palette mapping. */
            gpu_set_sprite_palette0(val);
            break;
        case 0xff49:
            /* OBJ1 palette mapping. */
            gpu_set_sprite_palette1(val);
            break;
        case 0xff4a:
            /* Window Y position. */
            GPU.window_y = val;
            break;
        case 0xff4b:
            /* Window X position. */
            GPU.window_x = val;
            break;
        default:
            printf("gpu_write_byte: not implemented: 0x%04x\n", addr);
            break;
    }
}

static uint32_t gpu_get_tile_id(int mapoffs, int map_row, int map_col)
{
    uint32_t tile_id;
    if (GPU.bg_tile_set) {
        /* Unsigned tile region: 0 to 255. */
        tile_id = (uint32_t)GPU.vram[mapoffs + map_row + map_col];
    } else {
        /* Signed tile region: -128 to 127. */
        int tile_signed_id = (int)GPU.vram[mapoffs + map_row + map_col];
        /* Adjust id for the 0x8000 - 0x97ff range. */
        tile_id = (uint32_t)(256 + tile_signed_id);
    }
    return tile_id;
}

typedef struct {
    uint8_t data_h;
    uint8_t data_l;
} tile_line_t;

static tile_line_t get_tile_line(uint32_t tile_id, int y)
{
    tile_line_t tile_line;
    /* Get tile index for the correct line of the tile. Each tile is 16 bytes
     * long. */
    uint32_t tile_line_id = tile_id * 16 + (uint32_t)((y % 8) * 2);
    /* Get tile line data: Each tile line takes 2 bytes. */
    tile_line.data_l = GPU.vram[tile_line_id];
    tile_line.data_h = GPU.vram[tile_line_id + 1];
    return tile_line;
}

static tile_line_t get_tile_line_sprite(uint32_t tile_id, int sy, bool yflip,
                                        uint32_t ysize)
{
    tile_line_t tile_line;
    uint32_t tile_y = (uint32_t)(GPU.scanline - sy);
    if (yflip) {
        tile_y = ysize - tile_y;
    }
    uint32_t tile_line_id = tile_id * 16 + tile_y * 2;
    /* Get tile line data: Each tile line takes 2 bytes. */
    tile_line.data_l = GPU.vram[tile_line_id];
    tile_line.data_h = GPU.vram[tile_line_id + 1];
    return tile_line;
}

static uint32_t gpu_get_tile_color(tile_line_t tile_line, int tile_x)
{
    /* Get bit index for pixel. */
    uint32_t color_bit = (uint32_t)(7 - tile_x);
    /* Get color number. */
    uint32_t color_num = (tile_line.data_h & (1 << color_bit)) ? 2 : 0;
    color_num |= (tile_line.data_l & (1 << color_bit)) ? 1 : 0;
    return color_num;
}

static void gpu_update_fb_bg(uint8_t *scanline_row)
{
    int mapoffs, bg_x, bg_y;
    /* Get background xy coordinate, and Get map offset relative to vram array.
     */
    if (GPU.window_on && GPU.window_y <= GPU.scanline) {
        bg_x = -GPU.window_x;
        bg_y = GPU.scanline - GPU.window_y;
        mapoffs = (GPU.window_tile_map) ? 0x1c00 : 0x1800;
    } else {
        bg_x = GPU.scroll_x;
        bg_y = GPU.scanline + GPU.scroll_y;
        mapoffs = (GPU.bg_tile_map) ? 0x1c00 : 0x1800;
    }
    /* Map row offset. */
    int map_row = (bg_y / 8) * 32;
    uint32_t screen_x = 0;
    uint32_t pixeloffs = (uint32_t)GPU.scanline * 160;
    while (screen_x < 160) {
        int map_col = (bg_x / 8);
        /* Get tile index adjusted for the 0x8000 - 0x97ff range. */
        uint32_t tile_id = gpu_get_tile_id(mapoffs, map_row, map_col);
        /* Get tile line data. */
        tile_line_t tile_line = get_tile_line(tile_id, bg_y);
        /* Iterate over remaining pixels of the tile. */
        for (int tile_x = screen_x % 8; tile_x < 8; ++tile_x) {
            /* Get tile color number for coordinate. */
            uint32_t color_num = gpu_get_tile_color(tile_line, tile_x);
            scanline_row[screen_x] = (uint8_t)color_num;
            /* Copy color to frame buffer. */
            GPU.framebuffer[pixeloffs].r = GPU.bg_palette[color_num].r;
            GPU.framebuffer[pixeloffs].g = GPU.bg_palette[color_num].g;
            GPU.framebuffer[pixeloffs].b = GPU.bg_palette[color_num].b;
            ++pixeloffs;
            ++screen_x;
            ++bg_x;
        }
    }
}

static void gpu_update_fb_sprite(uint8_t *scanline_row)
{
    uint32_t ysize = GPU.sprites_size ? 16 : 8;
    /* Iterate over all sprites. */
    for (uint32_t i = 0; i < 40; i++) {
        sprite_t sprite = ((sprite_t *)GPU.oam)[i];
        int sx = (int)sprite.x - 8;
        int sy = (int)sprite.y - 16;
        /* If sprite is on scanline. */
        if (sy <= GPU.scanline && (sy + (int)ysize) > GPU.scanline) {
            /* Get palette for this sprite. */
            rgb_t *pal = GPU.sprite_palette[sprite.palette];
            /* Get frame buffer pixel offset. */
            uint32_t pixeloffs = (uint32_t)(GPU.scanline * 160 + sx);
            tile_line_t tile_line =
                get_tile_line_sprite(sprite.tile, sy, sprite.yflip, ysize);
            /* Iterate over all tile pixels in the X-axis. */
            for (int tile_x = 0; tile_x < 8; tile_x++) {
                /* Calculate pixel x coordinate. */
                int px = sx + tile_x;
                /* If pixel is on screen. */
                if (px >= 0 && px < 160) {
                    /* Check if pixel is hidden. */
                    if (sprite.priority == 1 && scanline_row[px] != 0)
                        continue;
                    /* Check if sprite is x-flipped. */
                    int tile_x_flip = sprite.xflip ? 7 - tile_x : tile_x;
                    uint32_t color = gpu_get_tile_color(tile_line, tile_x_flip);
                    if (color != 0) {
                        /* Only show sprite of color not 0. */
                        GPU.framebuffer[pixeloffs].r = pal[color].r;
                        GPU.framebuffer[pixeloffs].g = pal[color].g;
                        GPU.framebuffer[pixeloffs].b = pal[color].b;
                    }
                    pixeloffs++;
                }
            }
        }
    }
}

static void gpu_render_scanline(void)
{
    /* Only render if display is on. */
    if (GPU.display_on) {
        uint8_t scanline_row[160];
        if (GPU.bg_on)
            gpu_update_fb_bg(scanline_row);
        if (GPU.sprites_on)
            gpu_update_fb_sprite(scanline_row);
    }
}

static void gpu_render_framebuffer(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRasterPos2i(-1, 1);
    glPixelZoom(GPU.zoom, -GPU.zoom);
    glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, GPU.framebuffer);
    glfwSwapBuffers(GPU.window);
    glfwPollEvents();
}

/* Check GPU FSM.
 * Note: All clock values are divided by 4.
 */
void gpu_step(uint32_t clock_step)
{
    GPU.modeclock += clock_step;
    switch (GPU.linemode) {
        case GPU_MODE_OAM:
            if (GPU.modeclock >= 20) {
                GPU.modeclock -= 20;
                GPU.linemode = GPU_MODE_VRAM;
            }
            break;
        case GPU_MODE_VRAM:
            if (GPU.modeclock >= 43) {
                GPU.modeclock -= 43;
                GPU.linemode = GPU_MODE_HBLANK;
                /* End of scanline. Write a scanline to framebuffer. */
                gpu_render_scanline();
            }
            break;
        case GPU_MODE_HBLANK:
            if (GPU.modeclock >= 51) {
                GPU.modeclock -= 51;
                GPU.scanline++;
                if (GPU.scanline == 143) {
                    GPU.linemode = GPU_MODE_VBLANK;
                    if (interrupt_is_enable(INTERRUPTS_VBLANK)) {
                        interrupt_set_flag_bit(INTERRUPTS_VBLANK);
                    }
                    gpu_render_framebuffer();
                } else {
                    GPU.linemode = GPU_MODE_OAM;
                }
            }
            break;
        case GPU_MODE_VBLANK:
            if (GPU.modeclock >= 114) {
                GPU.modeclock -= 114;
                GPU.scanline++;
                if (GPU.scanline > 153) {
                    GPU.scanline = 0;
                    GPU.linemode = GPU_MODE_OAM;
                }
            }
            break;
    }
}

void gpu_update_tile(uint16_t addr)
{
    /* TODO: check if can remove this. */
    uint16_t vram_index = addr & 0x1fff;
    uint16_t tile = (vram_index >> 4) & 511;
    uint16_t y = (vram_index >> 1) & 7;
    for (uint8_t x = 0; x < 8; ++x) {
        uint8_t bit_index = (uint8_t)(1 << (7 - x));
        GPU.tiles[tile][y][x] =
            (uint8_t)(((GPU.vram[vram_index] & bit_index) ? 1u : 0u) +
                      ((GPU.vram[vram_index + 1] & bit_index) ? 2u : 0u));
    }
}
