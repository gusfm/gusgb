#ifndef BG_MAP_WINDOW_H
#define BG_MAP_WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int bg_map_window_init(void);
void bg_map_window_finish(void);
void bg_map_window_render(void);
void bg_map_window_handle_events(SDL_Event *e);

#endif /* BG_MAP_WINDOW_H */
