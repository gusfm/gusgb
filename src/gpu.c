#include "gpu.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cartridge/cart.h"
#include "debug.h"
#include "interrupt.h"
#include "mmu.h"

typedef enum {
    GPU_MODE_HBLANK = 0,
    GPU_MODE_VBLANK = 1,
    GPU_MODE_OAM = 2,
    GPU_MODE_VRAM = 3,
} gpu_mode_e;

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
            uint8_t hflip : 1;
            uint8_t vflip : 1;
            uint8_t priority : 1;
        };
    };
} sprite_t;

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
    /* 0xff46 (DMA): DMA Transfer and Start Address (R/W) */
    struct {
        uint8_t reg;
        bool enabled;
        bool started;
        int byte;
        int clock;
    } oam_dma;
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
    unsigned int modeclock;
    uint8_t vram[2][0x2000]; /* Video RAM. */
    uint8_t oam[0xa0];       /* Sprite info. */
    color_t framebuffer[GB_SCREEN_WIDTH * GB_SCREEN_HEIGHT];
    color_t bg_palette[8 * 4];
    color_t sprite_palette[8 * 4];
    color_t bg_palette_data[8 * 4];
    color_t sprite_palette_data[8 * 4];
    unsigned int speed;
    bool lcd_disabled_frame_rendered;
    unsigned int lcd_disabled_clock;
} gpu_t;

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

int gpu_init(SDL_Renderer *ren, SDL_Texture *tex, render_callback_t cb)
{
    gpu_reset();
    assert(cb != NULL);
    GPU_GL.cb = cb;
    GPU_GL.ren = ren;
    GPU_GL.tex = tex;
    SDL_SetRenderDrawColor(GPU_GL.ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    return 0;
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
    return (!GPU.lcd_enable || GPU.mode_flag == GPU_MODE_HBLANK ||
           GPU.mode_flag == GPU_MODE_VBLANK) && GPU.oam_dma.byte == 0;
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

static void clear_line(int y)
{
    for (int x = 0; x < GB_SCREEN_WIDTH; ++x) {
        int px = y * GB_SCREEN_WIDTH + x;
        GPU.framebuffer[px] = dmg_palette[0];
    }
}

static void gpu_clear_screen(void)
{
    for (int y = 0; y < GB_SCREEN_HEIGHT; ++y) {
        clear_line(y);
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
        if (GPU.mode_flag != GPU_MODE_VBLANK)
            printf("WARNING: LCD should be disabled only during VBLANK: %d\n",
                   GPU.mode_flag);
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
    return (GPU.lcd_status & 0xfb) | (GPU.scanline == GPU.lyc ? 4u : 0u);
}

void gpu_write_stat(uint8_t val)
{
    GPU.lcd_status = (val & 0x78) | (GPU.lcd_status & 0x87);
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
    return GPU.oam_dma.reg;
}

void gpu_write_dma(uint8_t val)
{
    GPU.oam_dma.reg = val;
    GPU.oam_dma.enabled = true;
    if (GPU.oam_dma.byte > 0)
        GPU.oam_dma.started = false;
    GPU.oam_dma.byte = 0;
    GPU.oam_dma.clock = 0;
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

inline int gpu_get_tile_id(int mapoffs)
{
    /* Unsigned tile region: 0 to 255. */
    int tile_id = GPU.vram[0][mapoffs];
    if (!GPU.bg_tile_set) {
        /* Signed tile region: -128 to 127. */
        /* Adjust id for the 0x8000 - 0x97ff range. */
        tile_id = 256 + (int8_t)tile_id;
    }
    return tile_id;
}

inline tile_line_t gpu_get_tile_line(bg_attr_t attr, int tile_id, int y)
{
    tile_line_t tile_line;
    if (attr.vflip) {
        int ty = y & 7;
        y = y - ty + 7 - ty;
    }
    /* Get tile index for the correct line of the tile. Each tile is 16 bytes
     * long. */
    int tile_line_id = (tile_id << 4) + ((y & 7) << 1);
    /* Get tile line data: Each tile line takes 2 bytes. */
    tile_line.data_l = GPU.vram[attr.vram_bank][tile_line_id];
    tile_line.data_h = GPU.vram[attr.vram_bank][tile_line_id + 1];
    return tile_line;
}

inline bg_attr_t gpu_get_tile_attributes(int mapoffs)
{
    bg_attr_t bg_attr;
    if (cart_is_cgb()) {
        bg_attr.attributes = GPU.vram[1][mapoffs];
    } else {
        bg_attr.attributes = 0;
    }
    return bg_attr;
}

static inline tile_line_t get_tile_line_sprite(sprite_t *sprite, int sy,
                                               int ysize)
{
    tile_line_t tile_line;
    int tile_y = GPU.scanline - sy;
    if (sprite->vflip) {
        tile_y = ysize - tile_y - 1;
    }
    int tile_line_id = sprite->tile * 16 + tile_y * 2;
    /* Get tile line data: Each tile line takes 2 bytes. */
    tile_line.data_l = GPU.vram[sprite->cgb_vram_bank][tile_line_id];
    tile_line.data_h = GPU.vram[sprite->cgb_vram_bank][tile_line_id + 1];
    return tile_line;
}

inline int gpu_get_tile_color(tile_line_t tile_line, int tile_x, bool hflip)
{
    /* Get bit index for pixel. */
    int color_bit = hflip ? tile_x : 7 - tile_x;
    /* Get color number. */
    int color_num = (tile_line.data_h & (1 << color_bit)) >> color_bit;
    color_num <<= 1;
    color_num |= (tile_line.data_l & (1 << color_bit)) >> color_bit;
    return color_num;
}

struct scanline {
    uint8_t color;
    bool bg_priority;
};

static void update_fb_bg(struct scanline *line)
{
    int bg_x = GPU.scroll_x;
    int bg_y = (GPU.scanline + GPU.scroll_y) & 0xff;
    int map_x = (bg_x >> 3);
    int mapoffs = (GPU.bg_tile_map) ? 0x1c00 : 0x1800;
    int pixeloffs = GPU.scanline * GB_SCREEN_WIDTH;
    /* Map row offset: (bg_y / 8) * 32. */
    mapoffs += ((bg_y >> 3) << 5);
    for (int screen_x = 0; screen_x < GB_SCREEN_WIDTH; ++map_x) {
        map_x &= 0x1f;
        int mapoffs_tmp = mapoffs + map_x;
        /* Get tile index adjusted for the 0x8000 - 0x97ff range. */
        int tile_id = gpu_get_tile_id(mapoffs_tmp);
        bg_attr_t attr = gpu_get_tile_attributes(mapoffs_tmp);
        /* Get tile line data. */
        tile_line_t tile_line = gpu_get_tile_line(attr, tile_id, bg_y);
        /* Iterate over remaining pixels of the tile. */
        for (int tile_x = bg_x & 0x7; tile_x < 8 && screen_x < GB_SCREEN_WIDTH;
             ++tile_x) {
            /* Get tile color number for coordinate. */
            int color = gpu_get_tile_color(tile_line, tile_x, attr.hflip);
            line[screen_x].color = (uint8_t)color;
            line[screen_x].bg_priority = attr.priority;
            /* Copy color to frame buffer. */
            GPU.framebuffer[pixeloffs + screen_x] =
                GPU.bg_palette[((int)attr.pal_number << 2) + color];
            ++screen_x;
            ++bg_x;
        }
    }
}

static void update_fb_window(struct scanline *line)
{
    int bg_x = 0;
    int bg_y = GPU.scanline - GPU.window_y;
    int screen_x = GPU.window_x - 7;
    int mapoffs = (GPU.window_tile_map) ? 0x1c00 : 0x1800;
    int pixeloffs = GPU.scanline * GB_SCREEN_WIDTH;
    mapoffs += ((bg_y >> 3) << 5);
    if (screen_x < 0) {
        screen_x = 0;
        bg_x = 7 - GPU.window_x;
    }
    while (screen_x < GB_SCREEN_WIDTH) {
        int tile_id = gpu_get_tile_id(mapoffs);
        bg_attr_t attr = gpu_get_tile_attributes(mapoffs);
        tile_line_t tile_line = gpu_get_tile_line(attr, tile_id, bg_y);
        for (int tile_x = bg_x & 7; tile_x < 8 && screen_x < GB_SCREEN_WIDTH;
             ++tile_x) {
            int color = gpu_get_tile_color(tile_line, tile_x, attr.hflip);
            line[screen_x].color = (uint8_t)color;
            line[screen_x].bg_priority = attr.priority;
            GPU.framebuffer[pixeloffs + screen_x] =
                GPU.bg_palette[((int)attr.pal_number << 2) + color];
            ++screen_x;
            ++bg_x;
        }
        ++mapoffs;
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

static int get_lowest_prio_sprite(void)
{
    int ysize = GPU.obj_size ? 16 : 8;
    int sprites = 0, i;
    for (i = 0; i < 40 && sprites < 10; ++i) {
        sprite_t sprite = ((sprite_t *)GPU.oam)[i];
        int sy = sprite.y - 16;
        if (sy <= GPU.scanline && (sy + ysize) > GPU.scanline) {
            ++sprites;
        }
    }
    return i - 1;
}

static void update_fb_sprite(struct scanline *line)
{
    int ysize = GPU.obj_size ? 16 : 8;
    /* Iterate over the first 10 sprites on the scanline. */
    for (int i = get_lowest_prio_sprite(); i >= 0; --i) {
        sprite_t sprite = ((sprite_t *)GPU.oam)[i];
        int sx = sprite.x - 8;
        int sy = sprite.y - 16;
        /* If sprite is on scanline. */
        if (sy <= GPU.scanline && (sy + ysize) > GPU.scanline) {
            /* Get palette for this sprite. */
            color_t *pal = get_sprite_pal(&sprite);
            /* Get frame buffer pixel offset. */
            tile_line_t tile_line = get_tile_line_sprite(&sprite, sy, ysize);
            /* Iterate over all tile pixels in the X-axis. */
            for (int tile_x = 0; tile_x < 8; tile_x++) {
                /* Calculate pixel x coordinate. */
                int px = sx + tile_x;
                /* Calculate frame buffer pixel offset. */
                int pixeloffs = GPU.scanline * GB_SCREEN_WIDTH + px;
                /* If pixel is on screen. */
                if (px >= 0 && px < GB_SCREEN_WIDTH) {
                    /* Check if pixel is hidden. */
                    if (GPU.bg_display && line[px].color != 0 &&
                        (line[px].bg_priority || sprite.priority == 1))
                        continue;
                    int color =
                        gpu_get_tile_color(tile_line, tile_x, sprite.hflip);
                    if (color != 0) {
                        /* Only show sprite of color not 0. */
                        GPU.framebuffer[pixeloffs] = pal[color];
                    }
                }
            }
        }
    }
}

static void render_scanline(void)
{
    struct scanline line[GB_SCREEN_WIDTH];
    if (cart_is_cgb()) {
        /* In CGB mode when Bit 0 is cleared, the background and window
         * lose their priority. */
        update_fb_bg(line);
        if (GPU.window_enable && GPU.window_y <= GPU.scanline)
            update_fb_window(line);
    } else {
        if (GPU.bg_display) {
            update_fb_bg(line);
        } else {
            clear_line(GPU.scanline);
        }
        if (GPU.window_enable && GPU.window_y <= GPU.scanline)
            update_fb_window(line);
    }
    if (GPU.obj_enable)
        update_fb_sprite(line);
}

void gpu_render_framebuffer(void)
{
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
    {204, 456, 80, 172},
    {408, 912, 164, 344},
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
                render_scanline();
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
                    if (GPU.coincidence_int && GPU.scanline == GPU.lyc) {
                        interrupt_raise(INTERRUPTS_LCDSTAT);
                    }
                    gpu_change_mode(GPU_MODE_OAM);
                } else {
                    GPU.scanline++;
                    if (GPU.coincidence_int && GPU.scanline == GPU.lyc) {
                        interrupt_raise(INTERRUPTS_LCDSTAT);
                    }
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

static void gpu_dma_transfer(unsigned int clock_step)
{
    if (!GPU.oam_dma.started) {
        /* Ignore instruction that enabled DMA. */
        GPU.oam_dma.started = true;
        return;
    }
    GPU.oam_dma.clock += clock_step;
    while (GPU.oam_dma.clock >= 4) {
        int i = GPU.oam_dma.byte++;
        GPU.oam[i] = mmu_read_byte_dma((GPU.oam_dma.reg << 8) + i);
        GPU.oam_dma.clock -= 4;
        if (GPU.oam_dma.byte == 40 * 4) {
            GPU.oam_dma.enabled = false;
            GPU.oam_dma.byte = 0;
            GPU.oam_dma.clock = 0;
            break;
        }
    }
}

void gpu_tick(unsigned int clock_step)
{
    if (GPU.oam_dma.enabled) {
        gpu_dma_transfer(clock_step);
    }
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
    for (int i = 0; i < 40; i++) {
        sprite_t s = ((sprite_t *)GPU.oam)[i];
        printf("[$%.4x] ", 0xfe00 + i * 4);
        printf("sprite %2u: ", i);
        printf("%.2hhx %.2hhx %.2hhx %.2hhx: ", s.y, s.x, s.tile, s.options);
        printf("xy=(%3d, %3d) ", (int)s.x - 8, (int)s.y - 16);
        printf("options=[palette=%d, hflip=%d, vflip=%d, priority=%d] ",
               s.palette, s.hflip, s.vflip, s.priority);
        printf("tile=0x%.2hhx tile addr=0x%.4x\n", s.tile,
               0x8000 + s.tile * 16);
    }
}

#ifdef DEBUGGER
color_t *gpu_get_bg_palette(void)
{
    return GPU.bg_palette;
}

color_t *gpu_get_sprite_palette(void)
{
    return GPU.sprite_palette;
}
#endif /* DEBUGGER */
