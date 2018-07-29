#include "bg_map_window.h"
#include "../gpu.h"
#include "button.h"
#include "text.h"

#define BG_MAP_WIN_WIDTH 512
#define BG_MAP_WIN_HEIGHT 512
#define BG_WIDTH 256
#define BG_HEIGHT 256

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

static uint32_t window_id;
static color_t framebuffer[BG_WIDTH * BG_HEIGHT];
static uint8_t *vram;
static bool map_tile_selected = false;
static SDL_Rect map_tile_rect = {0, 0, 16, 16};
static SDL_Rect scroll_rect1 = {0, 0, GB_SCREEN_WIDTH * 2,
                                GB_SCREEN_HEIGHT * 2};
static SDL_Rect scroll_rect2 = {0, 0, GB_SCREEN_WIDTH * 2,
                                GB_SCREEN_HEIGHT * 2};

static uint32_t get_tile_id(int mapoffs)
{
    uint32_t tile_id = vram[mapoffs];
    uint8_t lcdc = gpu_read_lcdc();
    if (!(lcdc & 0x10))
        tile_id = (uint32_t)(256 + (int8_t)tile_id);
    return tile_id;
}

static int get_map_offset(void)
{
    uint8_t lcdc = gpu_read_lcdc();
    return (lcdc & 0x8) ? 0x1c00 : 0x1800;
}

static void update_framebuffer(void)
{
    color_t *bg_palette = gpu_get_bg_palette();
    int mapoffs = get_map_offset();
    /* 32 x 32 tiles */
    for (int my = 0; my < 32; ++my) {
        for (int mx = 0; mx < 32; ++mx) {
            uint32_t tile_id = get_tile_id(mapoffs);
            bg_attr_t attr = gpu_get_tile_attributes(tile_id);
            /* 8 x 8 pixels */
            for (int ty = 0; ty < 8; ++ty) {
                tile_line_t line = gpu_get_tile_line(attr, tile_id, ty);
                for (int tx = 0; tx < 8; ++tx) {
                    uint32_t c = gpu_get_tile_color(line, tx, attr.hflip);
                    int i = 256 * 8 * my + 8 * mx + 256 * ty + tx;
                    int pal_idx = ((uint32_t)attr.pal_number << 2) + c;
                    framebuffer[i] = bg_palette[pal_idx];
                }
            }
            ++mapoffs;
        }
    }
}

static void update_scroll(void)
{
    int x = gpu_read_scx();
    int y = gpu_read_scy();
    scroll_rect1.x = x * 2;
    scroll_rect1.y = y * 2;
    scroll_rect2.x = scroll_rect1.x;
    scroll_rect2.y = scroll_rect1.y;
    if (x + GB_SCREEN_WIDTH > BG_WIDTH) {
        scroll_rect2.x = (x - 256) * 2;
    }
    if (y + GB_SCREEN_HEIGHT > BG_HEIGHT) {
        scroll_rect2.y = (y - 256) * 2;
    }
}

int bg_map_window_init(void)
{
    int width = BG_MAP_WIN_WIDTH;
    int height = BG_MAP_WIN_HEIGHT;

    window = SDL_CreateWindow("BG map", 0, 0, width, height, 0);
    if (window == NULL) {
        fprintf(stderr, "%s:%d: %s\n", __func__, __LINE__, SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "%s:%d: %s\n", __func__, __LINE__, SDL_GetError());
        return -1;
    }

    texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING, BG_WIDTH, BG_HEIGHT);
    if (texture == NULL) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        fprintf(stderr, "%s:%d: %s\n", __func__, __LINE__, SDL_GetError());
        return -1;
    }

    window_id = SDL_GetWindowID(window);
    vram = gpu_get_vram();
    update_framebuffer();
    update_scroll();

    return 0;
}

void bg_map_window_finish(void)
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

static void render_scroll(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &scroll_rect1);
    SDL_RenderDrawRect(renderer, &scroll_rect2);
}

static void render_selected(void)
{
    if (map_tile_selected) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &map_tile_rect);
    }
}

void bg_map_window_render(void)
{
    SDL_SetRenderDrawColor(renderer, 0xe7, 0xe7, 0xe6, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, NULL, framebuffer, BG_WIDTH * 4);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    render_scroll();
    render_selected();
    SDL_RenderPresent(renderer);
}

static void select_map_tile(int x, int y)
{
    int mx = x / 16;
    int my = y / 16;
    int mapoffs = get_map_offset() + my * 32 + mx;
    uint32_t tile_id = get_tile_id(mapoffs);
    uint32_t tile_addr = 0x8000 + (tile_id << 4);
    bg_attr_t attr = gpu_get_tile_attributes(tile_id);
    map_tile_rect.x = mx * 16;
    map_tile_rect.y = my * 16;
    map_tile_selected = true;
    printf("x=%d, y=%d, map_addr=0x%x, tile_id=0x%x, tile_addr=0x%x\n", mx, my,
           0x8000 + mapoffs, tile_id, tile_addr);
    printf("attributes: palette=%d, hflip=%d, vflip=%d, priority=%d\n",
           attr.pal_number, attr.hflip, attr.vflip, attr.priority);
    for (int i = 0; i < 8; ++i) {
        tile_line_t line = gpu_get_tile_line(attr, tile_id, i);
        printf("%.2hhx %.2hhx ", line.data_h, line.data_l);
        for (int j = 0; j < 8; ++j) {
            uint32_t color_num = gpu_get_tile_color(line, j, attr.hflip);
            printf("%u", color_num);
        }
        printf("\n");
    }
}

void bg_map_window_handle_events(SDL_Event *e)
{
    switch (e->type) {
        case SDL_MOUSEBUTTONUP:
            if (e->button.windowID == window_id) {
                select_map_tile(e->button.x, e->button.y);
            }
            break;
    }
}
