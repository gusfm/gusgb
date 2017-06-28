#include "gpu.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cart.h"
#include "interrupt.h"
#include "mmu.h"
#include "utils/debug.h"

gpu_t GPU;
gpu_gl_t GPU_GL;

const rgb_t g_palette[4] = {
    {0xe0, 0xf8, 0xd0},  // off
    {0x88, 0xc0, 0x70},  // 33% on
    {0x34, 0x68, 0x56},  // 66% on
    {0x00, 0x00, 0x00},  // on
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
    GPU.mode_flag = GPU_MODE_OAM;
    GPU.vram_bank = 0;
    GPU.speed = 1;
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
    /* Fix GPU startup values. */
    /* TODO: Check if it's not a bug. */
    GPU.lcd_status = 0x85;
    GPU.scanline = 0x00;
    if (!cart_is_cgb()) {
        gpu_write_byte(0xff48, 0xFF);
        gpu_write_byte(0xff49, 0xFF);
    }
}

void gpu_gl_disable(void)
{
    GPU_GL.gl_enabled = false;
}

rgb_t *gpu_get_framebuffer(void)
{
    return GPU.framebuffer;
}

/* Check if the CPU can access VRAM. */
static bool gpu_check_vram_io(void)
{
    return !GPU.lcd_enable || GPU.mode_flag != GPU_MODE_VRAM;
}

uint8_t gpu_read_vram(uint16_t addr)
{
    if (gpu_check_vram_io())
        return GPU.vram[GPU.vram_bank][addr & 0x1fff];
    else
        return 0xFF;
}

void gpu_write_vram(uint16_t addr, uint8_t val)
{
    if (gpu_check_vram_io())
        GPU.vram[GPU.vram_bank][addr & 0x1fff] = val;
}

/* Check if the CPU can access OAM. */
static bool gpu_check_oam_io(void)
{
    return !GPU.lcd_enable || GPU.mode_flag == GPU_MODE_HBLANK ||
           GPU.mode_flag == GPU_MODE_VBLANK;
}

uint8_t gpu_read_oam(uint16_t addr)
{
    if (gpu_check_oam_io()) {
        return GPU.oam[addr & 0xff];
    } else {
        return 0xFF;
    }
}

void gpu_write_oam(uint16_t addr, uint8_t val)
{
    if (gpu_check_oam_io())
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
        GPU.sprite_palette[i] = g_palette[(value >> (i << 1)) & 3];
    }
}

static void gpu_set_sprite_palette1(uint8_t value)
{
    for (int i = 0; i < 4; ++i) {
        GPU.sprite_palette[i + 4] = g_palette[(value >> (i << 1)) & 3];
    }
}

static void gpu_set_cgb_bg_palette(uint8_t value)
{
    uint8_t reg = GPU.cgb_bg_pal_idx;
    unsigned int i = reg & 0x3f;
    GPU.cgb_bg_pal_data[i] = value;
    uint16_t color = (GPU.cgb_bg_pal_data[i + 1] << 8) | value;
    rgb_t rgb;
    rgb.r = (color & 0x1f) * 255 / 31;
    rgb.g = ((color >> 5) & 0x1f) * 255 / 31;
    rgb.b = ((color >> 10) & 0x1f) * 255 / 31;
    GPU.bg_palette[i >> 1] = rgb;
    /* Auto increment index. */
    GPU.cgb_bg_pal_idx = (reg & ~0x3f) | ((i + (reg >> 7)) & 0x3f);
}

static void gpu_set_cgb_sprite_palette(uint8_t value)
{
    uint8_t reg = GPU.cgb_sprite_pal_idx;
    unsigned int i = reg & 0x3f;
    GPU.cgb_sprite_pal_data[i] = value;
    uint16_t color = (GPU.cgb_sprite_pal_data[i + 1] << 8) | value;
    rgb_t rgb;
    rgb.r = (color & 0x1f) * 255 / 31;
    rgb.g = ((color >> 5) & 0x1f) * 255 / 31;
    rgb.b = ((color >> 10) & 0x1f) * 255 / 31;
    GPU.sprite_palette[i >> 1] = rgb;
    /* Auto increment index. */
    GPU.cgb_sprite_pal_idx = (reg & ~0x3f) | ((i + (reg >> 7)) & 0x3f);
}

uint8_t gpu_read_byte(uint16_t addr)
{
    switch (addr) {
        case 0xff40:
            return GPU.lcd_control;
        case 0xff41:
            return (uint8_t)(GPU.lcd_status |
                             (GPU.scanline == GPU.raster ? 4u : 0u) |
                             GPU.mode_flag);
        case 0xff42:
            return GPU.scroll_y;
        case 0xff43:
            return GPU.scroll_x;
        case 0xff44:
            return GPU.scanline;
        case 0xff45:
            return GPU.raster;
        case 0xff46:
            return GPU.dma;
        case 0xff47:
            return GPU.bgp;
        case 0xff48:
            return GPU.obp0;
        case 0xff49:
            return GPU.obp1;
        case 0xff4a:
            return GPU.window_y;
        case 0xff4b:
            return GPU.window_x;
        case 0xff4f:
            return GPU.vram_bank;
        case 0xff68:
            return GPU.cgb_bg_pal_idx;
        case 0xff69:
            return GPU.cgb_bg_pal_data[GPU.cgb_bg_pal_idx & 0x3f];
        case 0xff6a:
            return GPU.cgb_sprite_pal_idx;
        case 0xff6b:
            return GPU.cgb_sprite_pal_data[GPU.cgb_sprite_pal_idx & 0x3f];
        default:
            fprintf(stderr, "gpu_read_byte: not implemented: 0x%04x\n", addr);
            abort();
    }
}

static void gpu_clear_line(int y)
{
    for (int x = 0; x < 160; ++x) {
        int px = y * 160 + x;
        GPU.framebuffer[px] = g_palette[0];
    }
}

static void gpu_clear_screen(void)
{
    for (int y = 0; y < 144; ++y) {
        gpu_clear_line(y);
    }
}

void gpu_write_byte(uint16_t addr, uint8_t val)
{
    switch (addr) {
        case 0xff40:
            if (GPU.lcd_enable && !(val & 0x80)) {
                assert(GPU.mode_flag == GPU_MODE_VBLANK);
                /* Clear screen when it's disabled. */
                gpu_clear_screen();
            }
            GPU.lcd_control = val;
            break;
        case 0xff41:
            GPU.lcd_status = (val & 0xf8) | (GPU.lcd_status & 0x07);
            break;
        case 0xff42:
            GPU.scroll_y = val;
            break;
        case 0xff43:
            GPU.scroll_x = val;
            break;
        case 0xff44:
            break;
        case 0xff45:
            GPU.raster = val;
            break;
        case 0xff46:
            /* OAM DMA. */
            GPU.dma = val;
            for (int i = 0; i < 160; i++) {
                uint16_t dma_addr = (uint16_t)((val << 8) + i);
                uint8_t v = mmu_read_byte(dma_addr);
                GPU.oam[i] = v;
            }
            break;
        case 0xff47:
            /* BG palette mapping. */
            GPU.bgp = val;
            gpu_set_bg_palette(val);
            break;
        case 0xff48:
            /* OBJ0 palette mapping. */
            GPU.obp0 = val;
            gpu_set_sprite_palette0(val);
            break;
        case 0xff49:
            /* OBJ1 palette mapping. */
            GPU.obp1 = val;
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
        case 0xff4f:
            /* Select VRAM bank. */
            GPU.vram_bank = val & 1;
            break;
        case 0xff68:
            /* GBC: BG palette index. */
            GPU.cgb_bg_pal_idx = val;
            break;
        case 0xff69:
            /* GBC: BG palette data. */
            gpu_set_cgb_bg_palette(val);
            break;
        case 0xff6a:
            /* GBC: sprite palette index. */
            GPU.cgb_sprite_pal_idx = val;
            break;
        case 0xff6b:
            /* GBC: sprite palette data. */
            gpu_set_cgb_sprite_palette(val);
            break;
        case 0xff7f:
            fprintf(stderr, "gpu_write_byte: not implemented: 0x%04x=0x%02x\n",
                    addr, val);
            break;
        default:
            fprintf(stderr, "gpu_write_byte: not implemented: 0x%04x=0x%02x\n",
                    addr, val);
            abort();
    }
}

static uint32_t gpu_get_tile_id(int mapoffs, int map_row, int map_col)
{
    /* Unsigned tile region: 0 to 255. */
    uint32_t tile_id = GPU.vram[GPU.vram_bank][mapoffs + map_row + map_col];
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
    tile_line.data_l = GPU.vram[GPU.vram_bank][tile_line_id];
    tile_line.data_h = GPU.vram[GPU.vram_bank][tile_line_id + 1];
    return tile_line;
}

static tile_line_t get_tile_line_sprite(sprite_t *sprite, int sy, uint32_t ysize)
{
    tile_line_t tile_line;
    uint32_t tile_y = (uint32_t)(GPU.scanline - sy);
    if (sprite->yflip) {
        tile_y = ysize - tile_y;
    }
    uint32_t tile_line_id = sprite->tile * 16 + tile_y * 2;
    /* Get tile line data: Each tile line takes 2 bytes. */
    tile_line.data_l = GPU.vram[sprite->cgb_vram_bank][tile_line_id];
    tile_line.data_h = GPU.vram[sprite->cgb_vram_bank][tile_line_id + 1];
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
    if (GPU.window_enable && GPU.window_y <= GPU.scanline) {
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
            if (screen_x >= 160) {
                return;
            }
            /* Get tile color number for coordinate. */
            uint32_t color_num = gpu_get_tile_color(tile_line, tile_x);
            scanline_row[screen_x] = (uint8_t)color_num;
            /* Copy color to frame buffer. */
            GPU.framebuffer[pixeloffs + screen_x] = GPU.bg_palette[color_num];
            ++screen_x;
            ++bg_x;
        }
    }
}

static rgb_t *get_sprite_pal(sprite_t *sprite)
{
    rgb_t *pal;
    if (cart_is_cgb()) {
        pal = &GPU.sprite_palette[sprite->cgb_palette * 4];
    } else {
        pal = &GPU.sprite_palette[sprite->palette * 4];
    }
    return pal;
}

static void gpu_update_fb_sprite(uint8_t *scanline_row)
{
    uint32_t ysize = GPU.obj_size ? 16 : 8;
    int sprites = 0;
    /* Iterate over the first 10 sprites on the scanline. */
    for (uint32_t i = 0; i < 40; i++) {
        sprite_t sprite = ((sprite_t *)GPU.oam)[i];
        int sx = (int)sprite.x - 8;
        int sy = (int)sprite.y - 16;
        /* If sprite is on scanline. */
        if (sy <= GPU.scanline && (sy + (int)ysize) > GPU.scanline) {
            /* Get palette for this sprite. */
            rgb_t *pal = get_sprite_pal(&sprite);
            /* Get frame buffer pixel offset. */
            tile_line_t tile_line = get_tile_line_sprite(&sprite, sy, ysize);
            /* Iterate over all tile pixels in the X-axis. */
            for (int tile_x = 0; tile_x < 8; tile_x++) {
                /* Calculate pixel x coordinate. */
                int px = sx + tile_x;
                /* Calculate frame buffer pixel offset. */
                uint32_t pixeloffs = (uint32_t)(GPU.scanline * 160 + px);
                /* If pixel is on screen. */
                if (px >= 0 && px < 160) {
                    /* Check if pixel is hidden. */
                    if (GPU.bg_display && sprite.priority == 1 &&
                        scanline_row[px] != 0)
                        continue;
                    /* Check if sprite is x-flipped. */
                    int tile_x_flip = sprite.xflip ? 7 - tile_x : tile_x;
                    uint32_t color = gpu_get_tile_color(tile_line, tile_x_flip);
                    if (color != 0) {
                        /* Only show sprite of color not 0. */
                        GPU.framebuffer[pixeloffs] = pal[color];
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
    if (GPU.lcd_enable) {
        uint8_t scanline_row[160];
        if (cart_is_cgb()) {
            /* In CGB mode when Bit 0 is cleared, the background and window
             * lose their priority. */
            gpu_update_fb_bg(scanline_row);
        } else {
            if (GPU.bg_display) {
                gpu_update_fb_bg(scanline_row);
            } else {
                gpu_clear_line(GPU.scanline);
            }
        }
        if (GPU.obj_enable)
            gpu_update_fb_sprite(scanline_row);
    }
}

#define VIDEO

void gpu_render_framebuffer(void)
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
    GPU.mode_flag = new_mode;
    switch (new_mode) {
        case GPU_MODE_HBLANK:
            if (GPU.hblank_int)
                interrupt_set_flag_bit(INTERRUPTS_LCDSTAT);
            break;
        case GPU_MODE_VBLANK:
            if (interrupt_is_enable(INTERRUPTS_VBLANK))
                interrupt_set_flag_bit(INTERRUPTS_VBLANK);
            if (GPU.vblank_int)
                interrupt_set_flag_bit(INTERRUPTS_LCDSTAT);
            break;
        case GPU_MODE_OAM:
            if (GPU.oam_int)
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
    switch (GPU.mode_flag) {
        case GPU_MODE_OAM:
            /* Mode 2 takes between 77 and 83 clocks. */
            if (GPU.modeclock >= 80 * GPU.speed) {
                GPU.modeclock -= 80 * GPU.speed;
                gpu_change_mode(GPU_MODE_VRAM);
            }
            break;
        case GPU_MODE_VRAM:
            /* Mode 3 takes between 169 and 175 clocks. */
            if (GPU.modeclock >= 172 * GPU.speed) {
                GPU.modeclock -= 172 * GPU.speed;
                gpu_change_mode(GPU_MODE_HBLANK);
                /* End of scanline. Write a scanline to framebuffer. */
                gpu_render_scanline();
            }
            break;
        case GPU_MODE_HBLANK:
            /* Mode 0 takes between 201 and 207 clocks. */
            if (GPU.modeclock >= 204 * GPU.speed) {
                GPU.modeclock -= 204 * GPU.speed;
                GPU.scanline++;
                if (GPU.coincidence_int && GPU.scanline == GPU.raster) {
                    interrupt_set_flag_bit(INTERRUPTS_LCDSTAT);
                }
                if (GPU.scanline == 144) {
                    gpu_change_mode(GPU_MODE_VBLANK);
                    gpu_render_framebuffer();
                } else {
                    gpu_change_mode(GPU_MODE_OAM);
                }
            }
            break;
        case GPU_MODE_VBLANK:
            /* Mode 1 takes between 4560 clocks. */
            if (GPU.modeclock >= 456 * GPU.speed) {
                GPU.modeclock -= 456 * GPU.speed;
                if (GPU.scanline > 153) {
                    GPU.scanline = 0;
                    gpu_change_mode(GPU_MODE_OAM);
                } else {
                    GPU.scanline++;
                }
            }
            break;
    }
}

void gpu_change_speed(unsigned int speed)
{
    GPU.speed = speed + 1;
}

void gpu_dump(void)
{
    printf("GPU dump:\n");
    printf("lcd_enable=%hhu\n", GPU.lcd_enable);
    printf("bg_display=%hhu\n", GPU.bg_display);
    printf("obj_enable=%hhu\n", GPU.obj_enable);
    printf("window_enable=%hhu\n", GPU.window_enable);
    printf("window_x=%hhu\n", GPU.window_x);
    printf("window_y=%hhu\n", GPU.window_y);
    printf("scroll_x=%hhu\n", GPU.scroll_x);
    printf("scroll_y=%hhu\n", GPU.scroll_y);
    printf("scanline=%hhu\n", GPU.scanline);
    printf("BG palettes:\n");
    for (int i = 0; i < 8; ++i) {
        printf("BG %d: ", i);
        for (int j = 0; j < 4; ++j) {
            rgb_t *pal = &GPU.bg_palette[i * 4 + j];
            printf("(%u, %u, %u)", pal->r, pal->g, pal->b);
        }
        printf("\n");
    }
    for (uint16_t addr = 0xff40; addr <= 0xff4b; ++addr) {
        printf("[$%hx] = $%hhx\n", addr, gpu_read_byte(addr));
    }
    /* Dump OAM. */
    printf("OAM dump:\n");
    for (uint32_t i = 0; i < 40; i++) {
        sprite_t s = ((sprite_t *)GPU.oam)[i];
        printf("sprite %2u: ", i);
        printf("%.2hhx %.2hhx %.2hhx %.2hhx: ", s.y, s.x, s.tile, s.options);
        printf("xy=(%3d, %3d) ", (int)s.x - 8, (int)s.y - 16);
        printf("tile=0x%.2hhx ", s.tile);
        printf("OAM addr=0x%.4x, tile addr=0x%.4x ", 0xfe00 + i * 4,
               GPU.vram[GPU.vram_bank][s.tile]);
        printf("options=[palette=%d, xflip=%d, yflip=%d, priority=%d]\n",
               s.palette, s.xflip, s.yflip, s.priority);
    }
}
