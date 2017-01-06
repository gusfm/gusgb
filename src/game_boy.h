#ifndef __GAME_BOY_H__
#define __GAME_BOY_H__

#include <stdbool.h>
#include <stdint.h>

int gb_init(int width, int height, float window_zoom, const char *rom_path,
            bool debug, uint16_t breakpoint);
void gb_finish(void);
void gb_main(void);

#endif /* __GAME_BOY_H__ */
