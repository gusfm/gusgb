#include "gb_ai.h"
#include <stdio.h>
#include "ai/player.h"
#include "game_boy.h"

typedef struct {
    player_t *pl;
} gb_ai_t;

gb_ai_t gb_ai;

int gb_ai_init(int width, int height, float window_zoom, const char *rom_path)
{
    int ret = gb_init(width, height, window_zoom, rom_path);
    if (ret != 0)
        return ret;
    gb_ai.pl = player_create(160 * 144);
    return 0;
}

void gb_ai_main(void)
{
    while (!glfwWindowShouldClose(GB.window)) {
        cpu_emulate_cycle();
    }
}

void gb_ai_finish(void)
{
    if (GB.window) {
        glfwDestroyWindow(GB.window);
        glfwTerminate();
    }
    player_destroy(gb_ai.pl);
}
