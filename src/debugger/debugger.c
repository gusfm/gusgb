#include "debugger.h"
#include <SDL2/SDL.h>

enum debugger_view {
    DEBUGGER_PALETTE,
};

static SDL_Window *window;
static SDL_Renderer *renderer;
static enum debugger_view view;

int debugger_init(int width, int height)
{
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    /* Initialize debugger window. */
    window = SDL_CreateWindow("debugger", dm.w - width, 0, width, height, 0);
    if (window == NULL) {
        return -1;
    }
    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        return -1;
    }
    return 0;
}

void debugger_finish(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void render_palette_view(void)
{
    SDL_Rect rect = {100,100,100,100};
    SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &rect);
}

void debugger_render(void)
{
    SDL_SetRenderDrawColor(renderer, 0xe7, 0xe7, 0xe6, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    switch (view) {
        case DEBUGGER_PALETTE:
            render_palette_view();
            break;
    }
    SDL_RenderPresent(renderer);
}
