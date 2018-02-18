#ifndef GAME_BOY_H
#define GAME_BOY_H

#include <SDL.h>
#include <stdbool.h>

typedef struct {
    int width;
    int height;
    bool running;
    bool paused;
    SDL_Window *window;
} game_boy_t;

int gb_init(game_boy_t *gb, int scale, const char *rom_path);
void gb_finish(game_boy_t *gb);
void gb_main(game_boy_t *gb);

#endif /* GAME_BOY_H */
