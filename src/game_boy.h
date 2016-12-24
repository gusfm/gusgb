#ifndef __GAME_BOY_H__
#define __GAME_BOY_H__

#include <stdbool.h>

int gb_init(const char *rom_path, int width, int height, bool debug);
void gb_finish(void);
void gb_main(void);

#endif /* __GAME_BOY_H__ */
