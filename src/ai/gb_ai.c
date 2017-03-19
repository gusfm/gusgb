#include "gb_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include "ai/player.h"
#include "game_boy.h"
#include "keys.h"

typedef struct {
    player_t *pl;
    double *screen;
    unsigned int num_inputs;
    unsigned int num_outputs;
} gb_ai_t;

gb_ai_t gb_ai;

int gb_ai_init(int width, int height, float window_zoom, const char *rom_path)
{
    int ret = gb_init(width, height, window_zoom, rom_path, gb_ai_step);
    if (ret != 0)
        return ret;
    /* Disable gl rendering until the end of intro logo. */
    gpu_gl_disable();
    gb_ai.num_inputs = 160 * 144;
    gb_ai.num_outputs = 6;
    gb_ai.pl = player_create(gb_ai.num_inputs, gb_ai.num_outputs);
    gb_ai.screen = (double *)malloc(sizeof(double) * gb_ai.num_inputs);
    return 0;
}

static void get_input(double *inputs, unsigned int num_inputs)
{
    for (unsigned int i = 0; i < num_inputs; ++i) {
        inputs[i] = (double)(GB.gpu->framebuffer[i].r) / 255.0;
    }
}

static void check_player_keys(const double *outputs, unsigned int num_outputs)
{
    key_e key = KEY_B;
    for (unsigned int i = 0; i < num_outputs; ++i, ++key) {
        if (outputs[i] > 0.5) {
            /* Key pressed. */
            if (!key_check_pressed(key)) {
                printf("%s:%d: pressing key %d\n", __func__, __LINE__, key);
                key_press(key);
            }
        } else {
            /* Key released. */
            if (key_check_pressed(key)) {
                printf("%s:%d: releasing key %d\n", __func__, __LINE__, key);
                key_release(key);
            }
        }
    }
}

void gb_ai_step(void)
{
    static unsigned int cnt = 0;
    if (++cnt & 1) {
        /* Only calculate output every odd frame. */
        get_input(gb_ai.screen, gb_ai.num_inputs);
        const double *keys = player_output(gb_ai.pl, gb_ai.screen);
        check_player_keys(keys, gb_ai.num_outputs);
        /* TODO:
         * - Automatize game start;
         * - Only start player AI after starting game;
         * - Check if game over and update player fitness;
         */
    }
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
