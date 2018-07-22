#include "gpu.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cartridge/cart.h"
#include "debug.h"
#include "interrupt.h"
#include "mmu.h"

typedef struct {
    render_callback_t cb;
    SDL_Renderer *ren;
    SDL_Texture *tex;
} gpu_gl_t;

static gpu_t GPU;
static gpu_gl_t GPU_GL;

static const color_t dmg_palette[4] = {
#if (SDL_BYTE_ORDER == SDL_BIG_ENDIAN)
    {SDL_ALPHA_OPAQUE, 0xe0, 0xf8, 0xd0}, /* off */
    {SDL_ALPHA_OPAQUE, 0x88, 0xc0, 0x70}, /* 33% on */
    {SDL_ALPHA_OPAQUE, 0x34, 0x68, 0x56}, /* 66% on */
    {SDL_ALPHA_OPAQUE, 0x00, 0x00, 0x00}, /* on */
#else
    {0xd0, 0xf8, 0xe0, SDL_ALPHA_OPAQUE}, /* off */
    {0x70, 0xc0, 0x88, SDL_ALPHA_OPAQUE}, /* 33% on */
    {0x56, 0x68, 0x34, SDL_ALPHA_OPAQUE}, /* 66% on */
    {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE}, /* on */
#endif
};

int gpu_init(SDL_Window *win, render_callback_t cb)
{
    gpu_reset();
    assert(cb != NULL);
    GPU_GL.cb = cb;
    /* Create SDL renderer */
    GPU_GL.ren = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (GPU_GL.ren == NULL)
        return -1;
    /* Set device independent resolution for rendering */
    SDL_RenderSetLogicalSize(GPU_GL.ren, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT);
    /* Create SDL texture */
    GPU_GL.tex = SDL_CreateTexture(GPU_GL.ren, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING, GB_SCREEN_WIDTH,
                                   GB_SCREEN_HEIGHT);
    if (GPU_GL.tex == NULL)
        return -1;
    return 0;
}

void gpu_finish(void)
{
    SDL_DestroyTexture(GPU_GL.tex);
    SDL_DestroyRenderer(GPU_GL.ren);
}

void gpu_reset(void)
{
    memset(&GPU, 0, sizeof(GPU));
    GPU.lcd_control = 0x91;
    GPU.lcd_status = 0x85;
    gpu_write_bgp(0xfc);
    gpu_write_obp0(0xff);
    gpu_write_obp1(0xff);
    if (!cart_is_cgb()) {
        for (int i = 0; i < 4; ++i) {
            GPU.bg_palette_data[i] = dmg_palette[i];
            GPU.sprite_palette_data[i] = dmg_palette[i];
        }
    }
    GPU.speed = 0;
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

static void rgb5_to_rgb8(uint16_t rgb555, color_t *color)
{
    uint8_t c;
    color->a = SDL_ALPHA_OPAQUE;
    /* Red */
    c = (rgb555 & 0x1f);
    color->r = (c << 3) | (c >> 2);
    /* Green */
    c = (rgb555 >> 5) & 0x1f;
    color->g = (c << 3) | (c >> 2);
    /* Blue */
    c = (rgb555 >> 10) & 0x1f;
    color->b = (c << 3) | (c >> 2);
}

static void gpu_set_cgb_bg_palette(uint8_t value)
{
    uint8_t datah, datal;
    uint8_t reg = GPU.cgb_bg_pal_idx;
    unsigned int i = reg & 0x3f;
    GPU.cgb_bg_pal_data[i] = value;
    if (i & 1) {
        datah = value;
        datal = GPU.cgb_bg_pal_data[i - 1];
    } else {
        datah = GPU.cgb_bg_pal_data[i + 1];
        datal = value;
    }
    rgb5_to_rgb8((datah << 8) | datal, &GPU.bg_palette_data[i >> 1]);
    GPU.bg_palette[i >> 1] = GPU.bg_palette_data[i >> 1];
    /* Auto increment index. */
    GPU.cgb_bg_pal_idx = (reg & 0x80) | ((i + (reg >> 7)) & 0x3f);
}

static void gpu_set_cgb_sprite_palette(uint8_t value)
{
    uint8_t datah, datal;
    uint8_t reg = GPU.cgb_sprite_pal_idx;
    unsigned int i = reg & 0x3f;
    GPU.cgb_sprite_pal_data[i] = value;
    if (i & 1) {
        datah = value;
        datal = GPU.cgb_sprite_pal_data[i - 1];
    } else {
        datah = GPU.cgb_sprite_pal_data[i + 1];
        datal = value;
    }
    rgb5_to_rgb8((datah << 8) | datal, &GPU.sprite_palette_data[i >> 1]);
    GPU.sprite_palette[i >> 1] = GPU.sprite_palette_data[i >> 1];
    /* Auto increment index. */
    GPU.cgb_sprite_pal_idx = (reg & 0x80) | ((i + (reg >> 7)) & 0x3f);
}

static void gpu_clear_line(int y)
{
    for (int x = 0; x < GB_SCREEN_WIDTH; ++x) {
        int px = y * GB_SCREEN_WIDTH + x;
        GPU.framebuffer[px] = dmg_palette[0];
    }
}

static void gpu_clear_screen(void)
{
    for (int y = 0; y < GB_SCREEN_HEIGHT; ++y) {
        gpu_clear_line(y);
    }
}

uint8_t gpu_read_lcdc(void)
{
    return GPU.lcd_control;
}

void gpu_write_lcdc(uint8_t val)
{
    if (GPU.lcd_enable && (val & 0x80) == 0) {
        /* If disabling LCD */
        assert(GPU.mode_flag == GPU_MODE_VBLANK);
        gpu_clear_screen();
        GPU.lcd_disabled_clock = GPU.modeclock;
        GPU.lcd_disabled_frame_rendered = false;
        GPU.modeclock = 0;
        GPU.scanline = 0;
        GPU.mode_flag = GPU_MODE_OAM;
    }
    GPU.lcd_control = val;
}

uint8_t gpu_read_stat(void)
{
    return (uint8_t)(GPU.lcd_status | (GPU.scanline == GPU.lyc ? 4u : 0u) |
                     GPU.mode_flag);
}

void gpu_write_stat(uint8_t val)
{
    GPU.lcd_status = (val & 0xf8) | (GPU.lcd_status & 0x07);
}

uint8_t gpu_read_scy(void)
{
    return GPU.scroll_y;
}

void gpu_write_scy(uint8_t val)
{
    GPU.scroll_y = val;
}

uint8_t gpu_read_scx(void)
{
    return GPU.scroll_x;
}

void gpu_write_scx(uint8_t val)
{
    GPU.scroll_x = val;
}

uint8_t gpu_read_ly(void)
{
    return GPU.scanline;
}

uint8_t gpu_read_lyc(void)
{
    return GPU.lyc;
}

void gpu_write_lyc(uint8_t val)
{
    GPU.lyc = val;
}

uint8_t gpu_read_dma(void)
{
    return GPU.dma;
}

void gpu_write_dma(uint8_t val)
{
    GPU.dma = val;
    for (int i = 0; i < GB_SCREEN_WIDTH; i++) {
        uint16_t dma_addr = (uint16_t)((val << 8) + i);
        uint8_t v = mmu_read_byte_dma(dma_addr);
        GPU.oam[i] = v;
    }
}

uint8_t gpu_read_bgp(void)
{
    return GPU.bgp;
}

static void gpu_set_palette(color_t *dest, const color_t *src, uint8_t data)
{
    dest[0] = src[data >> 0 & 3];
    dest[1] = src[data >> 2 & 3];
    dest[2] = src[data >> 4 & 3];
    dest[3] = src[data >> 6 & 3];
}

void gpu_write_bgp(uint8_t val)
{
    GPU.bgp = val;
    gpu_set_palette(&GPU.bg_palette[0], GPU.bg_palette_data, val);
}

uint8_t gpu_read_obp0(void)
{
    return GPU.obp0;
}

void gpu_write_obp0(uint8_t val)
{
    GPU.obp0 = val;
    gpu_set_palette(&GPU.sprite_palette[0], GPU.sprite_palette_data, val);
}

uint8_t gpu_read_obp1(void)
{
    return GPU.obp1;
}

void gpu_write_obp1(uint8_t val)
{
    GPU.obp1 = val;
    gpu_set_palette(&GPU.sprite_palette[4], GPU.sprite_palette_data, val);
}

uint8_t gpu_read_wy(void)
{
    return GPU.window_y;
}

void gpu_write_wy(uint8_t val)
{
    GPU.window_y = val;
}

uint8_t gpu_read_wx(void)
{
    return GPU.window_x;
}

void gpu_write_wx(uint8_t val)
{
    GPU.window_x = val;
}

uint8_t gpu_read_vbk(void)
{
    return GPU.vram_bank;
}

void gpu_write_vbk(uint8_t val)
{
    GPU.vram_bank = val & 1;
}

uint8_t gpu_read_bgpi(void)
{
    return GPU.cgb_bg_pal_idx;
}

void gpu_write_bgpi(uint8_t val)
{
    GPU.cgb_bg_pal_idx = val;
}

uint8_t gpu_read_bgpd(void)
{
    return GPU.cgb_bg_pal_data[GPU.cgb_bg_pal_idx & 0x3f];
}

void gpu_write_bgpd(uint8_t val)
{
    gpu_set_cgb_bg_palette(val);
}

uint8_t gpu_read_obpi(void)
{
    return GPU.cgb_sprite_pal_idx;
}

void gpu_write_obpi(uint8_t val)
{
    GPU.cgb_sprite_pal_idx = val;
}

uint8_t gpu_read_obpd(void)
{
    return GPU.cgb_sprite_pal_data[GPU.cgb_sprite_pal_idx & 0x3f];
}

void gpu_write_obpd(uint8_t val)
{
    gpu_set_cgb_sprite_palette(val);
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

static uint32_t get_tile_palette(uint32_t tile_id)
{
    if (cart_is_cgb()) {
        cgb_bg_attr_t bg_attr;
        bg_attr.attributes = GPU.vram[1][tile_id];
        return bg_attr.pal_number;
    } else {
        return 0;
    }
}

static tile_line_t get_tile_line_sprite(sprite_t *sprite, int sy,
                                        uint32_t ysize)
{
    tile_line_t tile_line;
    uint32_t tile_y = (uint32_t)(GPU.scanline - sy);
    if (sprite->yflip) {
        tile_y = ysize - tile_y - 1;
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
        bg_x = 7 - GPU.window_x;
        bg_y = GPU.scanline - GPU.window_y;
        mapoffs = (GPU.window_tile_map) ? 0x1c00 : 0x1800;
    } else {
        bg_x = GPU.scroll_x;
        bg_y = (GPU.scanline + GPU.scroll_y) & 0xff;
        mapoffs = (GPU.bg_tile_map) ? 0x1c00 : 0x1800;
    }
    /* Map row offset: (bg_y / 8) * 32. */
    int map_row = (bg_y >> 3) << 5;
    uint32_t screen_x = 0;
    uint32_t pixeloffs = GPU.scanline * GB_SCREEN_WIDTH;
    while (screen_x < GB_SCREEN_WIDTH) {
        int map_col = (bg_x >> 3) & 0x1f;
        /* Get tile index adjusted for the 0x8000 - 0x97ff range. */
        uint32_t tile_id = gpu_get_tile_id(mapoffs, map_row, map_col);
        /* Get tile line data. */
        tile_line_t tile_line = get_tile_line(tile_id, bg_y);
        /* Get correct palette. */
        uint32_t palette_num = get_tile_palette(tile_id);
        /* Iterate over remaining pixels of the tile. */
        for (int tile_x = bg_x & 0x7; tile_x < 8; ++tile_x) {
            if (screen_x >= GB_SCREEN_WIDTH) {
                return;
            }
            /* Get tile color number for coordinate. */
            uint32_t color_num = gpu_get_tile_color(tile_line, tile_x);
            scanline_row[screen_x] = (uint8_t)color_num;
            /* Copy color to frame buffer. */
            GPU.framebuffer[pixeloffs + screen_x] =
                GPU.bg_palette[palette_num * 4 + color_num];
            ++screen_x;
            ++bg_x;
        }
    }
}

static color_t *get_sprite_pal(sprite_t *sprite)
{
    color_t *pal;
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
            color_t *pal = get_sprite_pal(&sprite);
            /* Get frame buffer pixel offset. */
            tile_line_t tile_line = get_tile_line_sprite(&sprite, sy, ysize);
            /* Iterate over all tile pixels in the X-axis. */
            for (int tile_x = 0; tile_x < 8; tile_x++) {
                /* Calculate pixel x coordinate. */
                int px = sx + tile_x;
                /* Calculate frame buffer pixel offset. */
                uint32_t pixeloffs = GPU.scanline * GB_SCREEN_WIDTH + px;
                /* If pixel is on screen. */
                if (px >= 0 && px < GB_SCREEN_WIDTH) {
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
    uint8_t scanline_row[GB_SCREEN_WIDTH];
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

void gpu_render_framebuffer(void)
{
    SDL_SetRenderDrawColor(GPU_GL.ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(GPU_GL.ren);
    SDL_UpdateTexture(GPU_GL.tex, NULL, GPU.framebuffer, GB_SCREEN_WIDTH * 4);
    SDL_RenderCopy(GPU_GL.ren, GPU_GL.tex, NULL, NULL);
    SDL_RenderPresent(GPU_GL.ren);
    GPU_GL.cb();
#ifdef FPS
    static uint32_t frames;
    static uint32_t last_time;
    ++frames;
    uint32_t time = SDL_GetTicks();
    if (time >= last_time + 1000) {
        printf("%u fps\n", frames);
        frames = 0;
        last_time = time;
    }
#endif
}

static void gpu_change_mode(gpu_mode_e new_mode)
{
    GPU.mode_flag = new_mode;
    switch (new_mode) {
        case GPU_MODE_HBLANK:
            if (GPU.hblank_int)
                interrupt_raise(INTERRUPTS_LCDSTAT);
            break;
        case GPU_MODE_VBLANK:
            if (interrupt_is_enable(INTERRUPTS_VBLANK))
                interrupt_raise(INTERRUPTS_VBLANK);
            if (GPU.vblank_int)
                interrupt_raise(INTERRUPTS_LCDSTAT);
            break;
        case GPU_MODE_OAM:
            if (GPU.oam_int)
                interrupt_raise(INTERRUPTS_LCDSTAT);
            break;
        case GPU_MODE_VRAM:
            break;
    }
}

static unsigned int mode_switch_clocks[2][4] = {
    {200, 456, 80, 172},
    {400, 912, 164, 344},
};

static void gpu_tick_lcd_enabled(unsigned int clock_step)
{
    GPU.modeclock += clock_step;
    unsigned int switch_clock = mode_switch_clocks[GPU.speed][GPU.mode_flag];
    switch (GPU.mode_flag) {
        case GPU_MODE_OAM:
            /* Mode 2 takes between 77 and 83 clocks. */
            if (GPU.modeclock >= switch_clock) {
                GPU.modeclock -= switch_clock;
                gpu_change_mode(GPU_MODE_VRAM);
            }
            break;
        case GPU_MODE_VRAM:
            /* Mode 3 takes between 169 and 175 clocks. */
            if (GPU.modeclock >= switch_clock) {
                GPU.modeclock -= switch_clock;
                gpu_change_mode(GPU_MODE_HBLANK);
                /* End of scanline. Write a scanline to framebuffer. */
                gpu_render_scanline();
            }
            break;
        case GPU_MODE_HBLANK:
            /* Mode 0 takes between 201 and 207 clocks. */
            if (GPU.modeclock >= switch_clock) {
                GPU.modeclock -= switch_clock;
                GPU.scanline++;
                if (GPU.coincidence_int && GPU.scanline == GPU.lyc) {
                    interrupt_raise(INTERRUPTS_LCDSTAT);
                }
                if (GPU.scanline == GB_SCREEN_HEIGHT) {
                    gpu_change_mode(GPU_MODE_VBLANK);
                    gpu_render_framebuffer();
                } else {
                    gpu_change_mode(GPU_MODE_OAM);
                }
            }
            break;
        case GPU_MODE_VBLANK:
            /* Mode 1 takes between 4560 clocks. */
            if (GPU.modeclock >= switch_clock) {
                GPU.modeclock -= switch_clock;
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

/* Render blank screen if LCD is disabled */
static void gpu_tick_lcd_disabled(unsigned int clock_step)
{
    GPU.lcd_disabled_clock += clock_step;
    if (!GPU.lcd_disabled_frame_rendered) {
        if (GPU.lcd_disabled_clock >= 144 * (456u << GPU.speed)) {
            GPU.lcd_disabled_frame_rendered = true;
            gpu_render_framebuffer();
        }
    } else {
        if (GPU.lcd_disabled_clock >= (144 + 10) * (456u << GPU.speed)) {
            GPU.lcd_disabled_frame_rendered = false;
            GPU.lcd_disabled_clock -= (144 + 10) * (456u << GPU.speed);
        }
    }
}

void gpu_tick(unsigned int clock_step)
{
    if (GPU.lcd_enable)
        gpu_tick_lcd_enabled(clock_step);
    else
        gpu_tick_lcd_disabled(clock_step);
}

void gpu_change_speed(unsigned int speed)
{
    GPU.speed = speed;
}

void gpu_dump(void)
{
    printf("GPU dump:\n");
    printf("[$ff40] lcd_control=0x%hhx:\n", GPU.lcd_control);
    printf("  bg_display=%hhu\n", GPU.bg_display);
    printf("  obj_enable=%hhu\n", GPU.obj_enable);
    printf("  obj_size=%hhu\n", GPU.obj_size);
    printf("  bg_tile_map=%hhu\n", GPU.bg_tile_map);
    printf("  bg_tile_set=%hhu\n", GPU.bg_tile_set);
    printf("  window_enable=%hhu\n", GPU.window_enable);
    printf("  window_tile_map=%hhu\n", GPU.window_tile_map);
    printf("  lcd_enable=%hhu\n", GPU.lcd_enable);
    printf("[$ff41] lcd_status=0x%hhx:\n", GPU.lcd_status);
    printf("[$ff42] scroll_y=%hhu\n", GPU.scroll_y);
    printf("[$ff43] scroll_x=%hhu\n", GPU.scroll_x);
    printf("[$ff44] scanline=%hhu\n", GPU.scanline);
    printf("[$ff45] lyc=%hhu\n", GPU.lyc);
    printf("[$ff4a] window_y=%hhu\n", GPU.window_y);
    printf("[$ff4b] window_x=%hhu\n", GPU.window_x);
    printf("BG palettes:\n");
    for (int i = 0; i < 8; ++i) {
        printf("BG %d: ", i);
        for (int j = 0; j < 4; ++j) {
            color_t *pal = &GPU.bg_palette[i * 4 + j];
            printf("(%u, %u, %u, %u)", pal->a, pal->r, pal->g, pal->b);
        }
        printf("\n");
    }
    /* Dump OAM. */
    printf("OAM dump:\n");
    for (uint32_t i = 0; i < 40; i++) {
        sprite_t s = ((sprite_t *)GPU.oam)[i];
        printf("[$%.4x] ", 0xfe00 + i * 4);
        printf("sprite %2u: ", i);
        printf("%.2hhx %.2hhx %.2hhx %.2hhx: ", s.y, s.x, s.tile, s.options);
        printf("xy=(%3d, %3d) ", (int)s.x - 8, (int)s.y - 16);
        printf("options=[palette=%d, xflip=%d, yflip=%d, priority=%d] ",
               s.palette, s.xflip, s.yflip, s.priority);
        printf("tile=0x%.2hhx tile addr=0x%.4x\n", s.tile,
               0x8000 + s.tile * 16);
    }
}
