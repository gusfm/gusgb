#include "palette_window.h"
#include "../gpu.h"
#include "button.h"
#include "text.h"

#define PALETTE_WIN_WIDTH 500
#define PALETTE_WIN_HEIGHT 420

static SDL_Window *window;
static SDL_Renderer *renderer;
static button_t bg_btn[8 * 4];
static button_t obj_btn[8 * 4];
static SDL_Color color = {0, 0, 0, 0};
static text_t *bg_label[8];
static text_t *obj_label[8];

int palette_window_init(SDL_DisplayMode *dm, TTF_Font *font)
{
    int width = PALETTE_WIN_WIDTH;
    int height = PALETTE_WIN_HEIGHT;

    window = SDL_CreateWindow("Palettes", dm->w - width, 0, width, height, 0);
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

    color_t *bg_palette = gpu_get_bg_palette();
    color_t *obj_palette = gpu_get_sprite_palette();

    for (int i = 0; i < 8; ++i) {
        char str[30];
        int x = 50;
        int y = 20;
        snprintf(str, sizeof(str), "BG %d", i);
        bg_label[i] = text_create(renderer, font, color, 10, 25 + i * 50, str);
        snprintf(str, sizeof(str), "OBJ %d", i);
        obj_label[i] =
            text_create(renderer, font, color, 245, 25 + i * 50, str);
        for (int j = 0; j < 4; ++j) {
            button_t *bg = &bg_btn[i * 4 + j];
            button_t *obj = &obj_btn[i * 4 + j];
            /* Background palettes */
            bg->x = x + j * 50;
            bg->y = y + i * 50;
            bg->w = 30;
            bg->h = 30;
            bg->pressed = true;
            bg->color = bg_palette[i * 4 + j];
            /* Sprite palettes */
            obj->x = 240 + x + j * 50;
            obj->y = y + i * 50;
            obj->w = 30;
            obj->h = 30;
            obj->pressed = true;
            obj->color = obj_palette[i * 4 + j];
        }
    }
    return 0;
}

void palette_window_finish(void)
{
    for (int i = 0; i < 8; ++i) {
        text_destroy(bg_label[i]);
        text_destroy(obj_label[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void palette_window_render(void)
{
    SDL_SetRenderDrawColor(renderer, 0xe7, 0xe7, 0xe6, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    for (int i = 0; i < 8; ++i) {
        text_render(bg_label[i]);
        text_render(obj_label[i]);
        for (int j = 0; j < 4; ++j) {
            button_render(renderer, &bg_btn[i * 4 + j]);
            button_render(renderer, &obj_btn[i * 4 + j]);
        }
    }
    SDL_RenderPresent(renderer);
}
