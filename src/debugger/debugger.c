#include "debugger.h"
#include <SDL2/SDL_ttf.h>
#include "bg_map_window.h"
#include "palette_window.h"
#include "tile_window.h"

enum debugger_windows {
    DEBUGGER_BG_MAP = 1,
    DEBUGGER_PALETTE = 2,
    DEBUGGER_TILES = 3,
};

static enum debugger_windows windows;
static TTF_Font *font;

int debugger_init(void)
{
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    if (TTF_Init() != 0) {
        printf("TTF_Init: %s\n", TTF_GetError());
        return -1;
    }
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 12);
    if (font == NULL) {
        TTF_Quit();
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        return -1;
    }
    windows = DEBUGGER_BG_MAP | DEBUGGER_PALETTE | DEBUGGER_TILES;
    if (bg_map_window_init() != 0) {
        TTF_CloseFont(font);
        TTF_Quit();
        return -1;
    }
    if (palette_window_init(&dm, font) != 0) {
        TTF_CloseFont(font);
        TTF_Quit();
        return -1;
    }
    if (tile_window_init(&dm) != 0) {
        TTF_CloseFont(font);
        TTF_Quit();
        return -1;
    }
    return 0;
}

void debugger_finish(void)
{
    if (windows & DEBUGGER_BG_MAP)
        bg_map_window_finish();
    if (windows & DEBUGGER_PALETTE)
        palette_window_finish();
    if (windows & DEBUGGER_TILES)
        tile_window_finish();
    TTF_CloseFont(font);
    TTF_Quit();
}

void debugger_render(void)
{
    if (windows & DEBUGGER_BG_MAP)
        bg_map_window_render();
    if (windows & DEBUGGER_PALETTE)
        palette_window_render();
    if (windows & DEBUGGER_TILES)
        tile_window_render();
}

void debugger_handle_events(SDL_Event *e)
{
    if (windows & DEBUGGER_BG_MAP)
        bg_map_window_handle_events(e);
    if (windows & DEBUGGER_TILES)
        tile_window_handle_events(e);
}
