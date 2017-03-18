#ifndef __GAME_BOY_H__
#define __GAME_BOY_H__

int gb_init(int width, int height, float window_zoom, const char *rom_path);
void gb_finish(void);
void gb_main(void);

#endif /* __GAME_BOY_H__ */
