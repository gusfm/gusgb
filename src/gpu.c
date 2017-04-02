#include "gpu.h"
#include <stdio.h>
#include <string.h>
#include "interrupt.h"
#include "mmu.h"
#include "utils/debug.h"

gpu_t GPU;
gpu_gl_t GPU_GL;

const rgb_t g_palette[4] = {
    {255, 255, 255},  // off
    {192, 192, 192},  // 33% on
    {96, 96, 96},     // 66% on
    {0, 0, 0},        // on
};

void gpu_init(float zoom)
{
    gpu_reset();
    GPU_GL.zoom = zoom;
    GPU_GL.callback = NULL;
    GPU_GL.gl_enabled = true;
    GPU_GL.window = NULL;
}

void gpu_reset(void)
{
    memset(&GPU, 0, sizeof(GPU));
    GPU.linemode = GPU_MODE_OAM;
    GPU_GL.gl_enabled = false;
}

gpu_t *gpu_get_instance(void)
{
    return &GPU;
}

void gpu_set_callback(render_callback_t cb)
{
    GPU_GL.callback = cb;
}

void gpu_set_glfw_window(GLFWwindow *window)
{
    GPU_GL.window = window;
}

void gpu_gl_enable(void)
{
    GPU_GL.gl_enabled = true;
}

void gpu_gl_disable(void)
{
    GPU_GL.gl_enabled = false;
}

rgb_t *gpu_get_framebuffer(void)
{
    return GPU.framebuffer;
}

static bool gpu_check_vram_io(void)
{
    // return !GPU.display_on || GPU.linemode == GPU_MODE_HBLANK || GPU.linemode
    // == GPU_MODE_VBLANK;
    // FIXME: Allow IO to vram for now since this was causing problems.
    return true;
}

uint8_t gpu_read_vram(uint16_t addr)
{
    if (gpu_check_vram_io())
        return GPU.vram[addr & 0x1fff];
    else
        return 0xFF;
}

void gpu_write_vram(uint16_t addr, uint8_t val)
{
    if (gpu_check_vram_io())
        GPU.vram[addr & 0x1fff] = val;
}

uint8_t gpu_read_oam(uint16_t addr)
{
    if (gpu_check_vram_io())
        return GPU.oam[addr & 0xff];
    else
        return 0xFF;
}

void gpu_write_oam(uint16_t addr, uint8_t val)
{
    if (gpu_check_vram_io())
        GPU.oam[addr & 0xff] = val;
}

static void gpu_set_bg_palette(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        GPU.bg_palette[i] = g_palette[(value >> (i << 1)) & 3];
    }
}

static void gpu_set_sprite_palette0(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        GPU.sprite_palette[0][i] = g_palette[(value >> (i << 1)) & 3];
    }
}

static void gpu_set_sprite_palette1(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        GPU.sprite_palette[1][i] = g_palette[(value >> (i << 1)) & 3];
    }
}

uint8_t gpu_read_byte(uint16_t addr)
{
    switch (addr) {
        case 0xff40:
            return GPU.control;
        case 0xff41:
            return (uint8_t)(GPU.lcd_status |
                             (GPU.scanline == GPU.raster ? 4u : 0u) |
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
            printd("gpu_read_byte: not implemented: 0x%04x\n", addr);
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
        case 0xff41:
            GPU.lcd_status =
                (uint8_t)((val & 0xf8u) | (GPU.lcd_status & 0x07u));
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
            printd("gpu_write_byte: not implemented: 0x%04x=0x%02x\n", addr,
                   val);
            break;
    }
}

static uint32_t gpu_get_tile_id(int mapoffs, int map_row, int map_col)
{
    /* Unsigned tile region: 0 to 255. */
    uint32_t tile_id = GPU.vram[mapoffs + map_row + map_col];
    if (!GPU.bg_tile_set) {
        /* Signed tile region: -128 to 127. */
        /* Adjust id for the 0x8000 - 0x97ff range. */
        tile_id = (uint32_t)(256 + (int8_t)tile_id);
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
    uint32_t tile_line_id = (tile_id << 4) + (uint32_t)((y & 7) << 1);
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
    /* Map row offset: (bg_y / 8) * 32. */
    int map_row = (bg_y >> 3) << 5;
    uint32_t screen_x = 0;
    uint32_t pixeloffs = GPU.scanline * 160u;
    while (screen_x < 160) {
        int map_col = (bg_x >> 3) & 0x1f;
        /* Get tile index adjusted for the 0x8000 - 0x97ff range. */
        uint32_t tile_id = gpu_get_tile_id(mapoffs, map_row, map_col);
        /* Get tile line data. */
        tile_line_t tile_line = get_tile_line(tile_id, bg_y);
        /* Iterate over remaining pixels of the tile. */
        for (int tile_x = bg_x & 0x7; tile_x < 8; ++tile_x) {
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
    int sprites = 0;
    /* Iterate over the first 10 sprites on the scanline. */
    for (uint32_t i = 0; i < 40; i++) {
        sprite_t sprite = ((sprite_t *)GPU.oam)[i];
        int sx = (int)sprite.x - 8;
        int sy = (int)sprite.y - 16;
        /* If sprite is on scanline. */
        if (sy <= GPU.scanline && (sy + (int)ysize) > GPU.scanline) {
            /* Get palette for this sprite. */
            rgb_t *pal = GPU.sprite_palette[sprite.palette];
            /* Get frame buffer pixel offset. */
            tile_line_t tile_line =
                get_tile_line_sprite(sprite.tile, sy, sprite.yflip, ysize);
            /* Iterate over all tile pixels in the X-axis. */
            for (int tile_x = 0; tile_x < 8; tile_x++) {
                /* Calculate pixel x coordinate. */
                int px = sx + tile_x;
                /* Calculate frame buffer pixel offset. */
                uint32_t pixeloffs = (uint32_t)(GPU.scanline * 160 + px);
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
                }
            }
            /* Only show 10 sprites for each scanline. */
            if (++sprites == 10)
                return;
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

//#define VIDEO

static void gpu_render_framebuffer(void)
{
    if (GPU_GL.gl_enabled) {
#ifdef VIDEO
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glRasterPos2i(-1, 1);
        glPixelZoom(GPU_GL.zoom, -GPU_GL.zoom);
        glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, GPU.framebuffer);
        glfwSwapBuffers(GPU_GL.window);
        glfwPollEvents();
#endif
        if (GPU_GL.callback)
            GPU_GL.callback();
    }
}

static void gpu_change_mode(gpu_mode_e new_mode)
{
    GPU.linemode = new_mode;
    switch (new_mode) {
        case GPU_MODE_HBLANK:
            if (GPU.lcd_status & LCD_INT_HBLANK)
                interrupt_set_flag_bit(INTERRUPTS_LCDSTAT);
            break;
        case GPU_MODE_VBLANK:
            if (interrupt_is_enable(INTERRUPTS_VBLANK))
                interrupt_set_flag_bit(INTERRUPTS_VBLANK);
            if (GPU.lcd_status & LCD_INT_VBLANK)
                interrupt_set_flag_bit(INTERRUPTS_LCDSTAT);
            break;
        case GPU_MODE_OAM:
            if (GPU.lcd_status & LCD_INT_OAM)
                interrupt_set_flag_bit(INTERRUPTS_LCDSTAT);
            break;
        case GPU_MODE_VRAM:
            break;
    }
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
                gpu_change_mode(GPU_MODE_VRAM);
            }
            break;
        case GPU_MODE_VRAM:
            if (GPU.modeclock >= 43) {
                GPU.modeclock -= 43;
                gpu_change_mode(GPU_MODE_HBLANK);
                /* End of scanline. Write a scanline to framebuffer. */
                gpu_render_scanline();
            }
            break;
        case GPU_MODE_HBLANK:
            if (GPU.modeclock >= 51) {
                GPU.modeclock -= 51;
                GPU.scanline++;
                if (GPU.scanline == GPU.raster &&
                    GPU.lcd_status & LCD_INT_LY_LYC) {
                    interrupt_set_flag_bit(INTERRUPTS_LCDSTAT);
                }
                if (GPU.scanline == 143) {
                    gpu_change_mode(GPU_MODE_VBLANK);
                    gpu_render_framebuffer();
                } else {
                    gpu_change_mode(GPU_MODE_OAM);
                }
            }
            break;
        case GPU_MODE_VBLANK:
            if (GPU.modeclock >= 114) {
                GPU.modeclock -= 114;
                GPU.scanline++;
                if (GPU.scanline > 153) {
                    GPU.scanline = 0;
                    gpu_change_mode(GPU_MODE_OAM);
                }
            }
            break;
    }
}
