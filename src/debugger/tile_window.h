#ifndef TILE_WINDOW_H
#define TILE_WINDOW_H

#include <SDL2/SDL.h>

int tile_window_init(SDL_DisplayMode *dm);
void tile_window_finish(void);
void tile_window_render(void);
void tile_window_handle_events(SDL_Event *e);

#endif /* TILE_WINDOW_H */
