#ifndef __GAME_BOY_H__
#define __GAME_BOY_H__

#include <GLFW/glfw3.h>
#include "cpu.h"
#include "gpu.h"

typedef struct {
    int width;
    int height;
    GLFWwindow *window;
} game_boy_t;

extern game_boy_t GB;

int gb_init(int width, int height, float window_zoom, const char *rom_path);
void gb_finish(void);
void gb_main(void);

#endif /* __GAME_BOY_H__ */
