#include "tile_window.h"
#include "../gpu.h"

#define TEXTURE_WIDTH 256
#define TEXTURE_HEIGHT 192

#define TILE_WIN_WIDTH (TEXTURE_WIDTH * 2)
#define TILE_WIN_HEIGHT (TEXTURE_HEIGHT * 2)

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;

static uint32_t window_id;
static color_t framebuffer[TEXTURE_WIDTH * TEXTURE_HEIGHT];
static bool tile_selected = false;
static SDL_Rect tile_rect = {0, 0, 16, 16};

static int get_map_offset(void)
{
    uint8_t lcdc = gpu_read_lcdc();
    return (lcdc & 0x8) ? 0x1c00 : 0x1800;
}

static bg_attr_t search_map_for_attr(int tile_id)
{
    bg_attr_t attr = {0};
    int mapoffs = get_map_offset();
    int end = mapoffs + 32 * 32;
    while (mapoffs < end) {
        if (tile_id == gpu_get_tile_id(mapoffs))
            return gpu_get_tile_attributes(mapoffs);
        ++mapoffs;
    }
    return attr;
}

static void update_framebuffer(void)
{
    color_t *bg_palette = gpu_get_bg_palette();
    int row = 0, column = 0;
    for (int tile_id = 0; tile_id < 384; ++tile_id) {
        bg_attr_t attr = search_map_for_attr(tile_id);
        if (column == 16) {
            ++row;
            column = 0;
        }
        for (int ty = 0; ty < 8; ++ty) {
            attr.vram_bank = 0;
            tile_line_t line = gpu_get_tile_line(attr, tile_id, ty);
            int i = TEXTURE_WIDTH * 8 * row + 8 * column + TEXTURE_WIDTH * ty;
            for (int tx = 0; tx < 8; ++tx) {
                int c = gpu_get_tile_color(line, tx, attr.hflip);
                int pal_idx = ((int)attr.pal_number << 2) + c;
                framebuffer[i + tx] = bg_palette[pal_idx];
            }
            attr.vram_bank = 1;
            line = gpu_get_tile_line(attr, tile_id, ty);
            for (int tx = 0; tx < 8; ++tx) {
                int c = gpu_get_tile_color(line, tx, attr.hflip);
                int pal_idx = ((int)attr.pal_number << 2) + c;
                framebuffer[i + tx + TEXTURE_WIDTH / 2] = bg_palette[pal_idx];
            }
        }
        ++column;
    }
}

int tile_window_init(SDL_DisplayMode *dm)
{
    int width = TILE_WIN_WIDTH;
    int height = TILE_WIN_HEIGHT;

    window = SDL_CreateWindow("Tiles", dm->w - width, 0, width, height, 0);
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

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, TEXTURE_WIDTH,
                                TEXTURE_HEIGHT);
    if (texture == NULL) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        fprintf(stderr, "%s:%d: %s\n", __func__, __LINE__, SDL_GetError());
        return -1;
    }

    window_id = SDL_GetWindowID(window);
    update_framebuffer();

    return 0;
}

void tile_window_finish(void)
{
    tile_selected = false;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

static void render_selected(void)
{
    if (tile_selected) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &tile_rect);
    }
}

void tile_window_render(void)
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_UpdateTexture(texture, NULL, framebuffer, TEXTURE_WIDTH * 4);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    render_selected();
    SDL_RenderPresent(renderer);
}

static void select_tile(int x, int y)
{
    int tx = x / 16;
    int ty = y / 16;
    int tile_id = ty * 16 + tx % 16;
    int tile_addr = 0x8000 + (tile_id << 4);
    tile_rect.x = tx * 16;
    tile_rect.y = ty * 16;
    tile_selected = true;
    printf("bank=%d, tile_id=0x%x, tile_addr=0x%x\n", tx > 15, tile_id,
           tile_addr);
}

void tile_window_handle_events(SDL_Event *e)
{
    switch (e->type) {
        case SDL_MOUSEBUTTONUP:
            if (e->button.windowID == window_id) {
                select_tile(e->button.x, e->button.y);
            }
            break;
    }
}
