#ifndef PALETTE_WINDOW_H
#define PALETTE_WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int palette_window_init(SDL_DisplayMode *dm, TTF_Font *font);
void palette_window_finish(void);
void palette_window_render(void);

#endif /* PALETTE_WINDOW_H */
