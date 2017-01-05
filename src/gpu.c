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

void gpu_init(void)
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

static void gpu_update_oam(uint16_t addr, uint8_t val)
{
    (void)addr;
    (void)val;
#if 0
    addr -= 0xFE00;
    uint16_t obj = addr >> 2;
    if (obj < 40) {
        switch (addr & 3) {
            case 0:
                GPU.objdata[obj].y = val - 16;
                break;
            case 1:
                GPU.objdata[obj].x = val - 8;
                break;
            case 2:
                if (GPU.objsize)
                    GPU.objdata[obj].tile = (val & 0xFE);
                else
                    GPU.objdata[obj].tile = val;
                break;
            case 3:
                GPU.objdata[obj].palette = (val & 0x10) ? 1 : 0;
                GPU.objdata[obj].xflip = (val & 0x20) ? 1 : 0;
                GPU.objdata[obj].yflip = (val & 0x40) ? 1 : 0;
                GPU.objdata[obj].prio = (val & 0x80) ? 1 : 0;
                break;
        }
    }
    GPU._objdatasorted = GPU.objdata;
    GPU._objdatasorted.sort(function(a, b) {
        if (a.x > b.x)
            return -1;
        if (a.num > b.num)
            return -1;
    });
#endif
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
    uint16_t gaddr = (uint16_t)(addr - 0xff40);
    switch (gaddr) {
        case 0x00:
            return GPU.control;
        case 0x01:
            return (uint8_t)((GPU.scanline == GPU.raster ? 4 : 0) |
                             GPU.linemode);
        case 0x02:
            return GPU.scroll_y;
        case 0x03:
            return GPU.scroll_x;
        case 0x04:
            return GPU.scanline;
        case 0x05:
            return GPU.raster;
        case 0xa:
            return GPU.window_y;
        case 0xb:
            return GPU.window_x;
        default:
            printf("gpu_read_byte: not implemented: 0x%04x\n", gaddr);
            return GPU.reg[gaddr];
    }
}

void gpu_write_byte(uint16_t addr, uint8_t val)
{
    uint16_t gaddr = (uint16_t)(addr - 0xff40);
    GPU.reg[gaddr] = val;
    switch (gaddr) {
        case 0x00:
            printf("GPU.control=0x%02x\n", val);
            GPU.control = val;
            break;
        case 0x02:
            GPU.scroll_y = val;
            break;
        case 0x03:
            GPU.scroll_x = val;
            break;
        case 0x05:
            GPU.raster = val;
            break;
        case 0x06:
            /* OAM DMA. */
            for (int i = 0; i < 160; i++) {
                uint8_t v = mmu_read_byte((uint8_t)((val << 8) + i));
                GPU.oam[i] = v;
                gpu_update_oam((uint8_t)(0xFE00 + i), v);
            }
            break;
        case 0x7:
            /* BG palette mapping. */
            gpu_set_bg_palette(val);
            break;
        case 0x8:
            /* OBJ0 palette mapping. */
            gpu_set_sprite_palette0(val);
            break;
        case 0x9:
            /* OBJ1 palette mapping. */
            gpu_set_sprite_palette1(val);
            break;
        case 0xa:
            /* Window Y position. */
            GPU.window_y = val;
            break;
        case 0xb:
            /* Window X position. */
            GPU.window_x = val;
            break;
        default:
            printf("gpu_write_byte: not implemented: 0x%04x\n", gaddr);
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
    /* Each tile is 16 bytes long. */
    return tile_id * 16;
}

static void gpu_update_fb_bg(uint8_t *scanline_row)
{
    int mapoffs, bg_x, bg_y;
    /* Get background xy coordinate, and Get map offset relative to vram array. */
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
        /* Get tile index ajusted for the 0x8000 - 0x97ff range. */
        uint32_t tile_id = gpu_get_tile_id(mapoffs, map_row, map_col);
        /* Get tile index for the correct line of the tile. */
        uint32_t tile_line_id = tile_id + (uint32_t)((bg_y % 8) * 2);
        /* Get tile line data: Each tile line takes 2 bytes. */
        uint8_t tile_l = GPU.vram[tile_line_id];
        uint8_t tile_h = GPU.vram[tile_line_id + 1];
        /* Iterate over remaining pixels of the tile. */
        for (int tile_x = screen_x % 8; tile_x < 8; ++tile_x) {
            /* Get bit index for pixel. */
            uint32_t color_bit = (uint32_t)(7 - tile_x);
            /* Get color number. */
            uint32_t color_num = (tile_h & (1 << color_bit)) ? 2 : 0;
            color_num |= (tile_l & (1 << color_bit)) ? 1 : 0;
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

static void gpu_update_fb_spr(uint8_t *scanline_row)
{
    int ysize = GPU.sprites_size ? 16 : 8;
    for (int i = 0; i < 40; i++) {
        sprite_t sprite = ((sprite_t *)GPU.oam)[i];

        int sx = sprite.x - 8;
        int sy = sprite.y - 16;

        if (sy <= GPU.scanline && (sy + ysize) > GPU.scanline) {
            rgb_t *pal = GPU.sprite_palette[sprite.palette];

            int pixel_offset = GPU.scanline * 160 + sx;

            uint8_t tile_row;
            if (sprite.y_flip)
                tile_row = (uint8_t)(7 - (GPU.scanline - sy - ysize));
            else
                tile_row = (uint8_t)(GPU.scanline - sy);

            for (int x = 0; x < 8; x++) {
                if (sx + x >= 0 && sx + x < 160 &&
                    (~sprite.priority || !scanline_row[sx + x])) {
                    uint8_t colour;

                    if (sprite.x_flip)
                        colour = GPU.tiles[sprite.tile][tile_row][7 - x];
                    else
                        colour = GPU.tiles[sprite.tile][tile_row][x];

                    if (colour) {
                        GPU.framebuffer[pixel_offset].r = pal[colour].r;
                        GPU.framebuffer[pixel_offset].g = pal[colour].g;
                        GPU.framebuffer[pixel_offset].b = pal[colour].b;
                    }

                    pixel_offset++;
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
            gpu_update_fb_spr(scanline_row);
	}
}

static void gpu_render_framebuffer(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRasterPos2i(-1, 1);
    glPixelZoom(1, -1);
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
