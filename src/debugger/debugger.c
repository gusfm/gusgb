#include "debugger.h"
#include <SDL2/SDL.h>
#include "palette_window.h"

enum debugger_windows {
    DEBUGGER_PALETTE = 1,
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
    font = TTF_OpenFont("/usr/share/fonts/noto/NotoSansMono-Thin.ttf", 12);
    if (font == NULL) {
        TTF_Quit();
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        return -1;
    }
    windows = DEBUGGER_PALETTE;
    if (palette_window_init(&dm, font) != 0) {
        TTF_CloseFont(font);
        TTF_Quit();
        return -1;
    }
    return 0;
}

void debugger_finish(void)
{
    if (windows & DEBUGGER_PALETTE)
        palette_window_finish();
    TTF_CloseFont(font);
    TTF_Quit();
}

void debugger_render(void)
{
    if (windows & DEBUGGER_PALETTE)
        palette_window_render();
}
