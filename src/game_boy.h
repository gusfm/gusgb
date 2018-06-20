#ifndef GAME_BOY_H
#define GAME_BOY_H

#include <SDL2/SDL.h>
#include <stdbool.h>

int gb_init(int scale, const char *rom_path, bool fullscreen);
void gb_finish(void);
void gb_main(void);

#endif /* GAME_BOY_H */
