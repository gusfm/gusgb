#include "gb_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include "ai/player.h"
#include "game_boy.h"
#include "keys.h"
#include "mmu.h"

/**
 * This file controls the game and AI initialization. For now it's hardcoded for
 * the Super Marioland game. In the future this should be configurable.
 */
typedef struct {
    player_t *pl;
    double *screen;
    unsigned int num_inputs;
    unsigned int num_outputs;
    cpu_t *cpu;
    gpu_t *gpu;
} gb_ai_t;

gb_ai_t gb_ai;

int gb_ai_init(int width, int height, float window_zoom, const char *rom_path)
{
    int ret = gb_init(width, height, window_zoom, rom_path, gb_ai_step);
    if (ret != 0)
        return ret;
    /* Get instance of CPU and GPU. */
    gb_ai.cpu = cpu_get_instance();
    gb_ai.gpu = gpu_get_instance();
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
        inputs[i] = (double)(gb_ai.gpu->framebuffer[i].r) / 255.0;
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
    }
}

static void gb_ai_wait_vblank(unsigned int num)
{
    unsigned int vblank_cnt = 0;
    do {
        cpu_emulate_cycle();
        if (gb_ai.cpu->reg.pc == 0x40)
            ++vblank_cnt;
    } while (vblank_cnt < num);
}

static void gb_ai_start_game(void)
{
    /* Execute until start game screen. */
    gb_ai_wait_vblank(2);
    /* Press start to start the game. */
    key_press(KEY_START);
    gb_ai_wait_vblank(1);
    key_release(KEY_START);
}

static unsigned int bcd_to_dec(uint8_t *bcd, unsigned int length)
{
    unsigned int dec = 0;
    unsigned int mult = 1;
    for (unsigned int i = 0; i < length; ++i) {
        dec += (unsigned int)(((bcd[i] & 0xf0) >> 4) * 10 + (bcd[i] & 0x0f)) *
               mult;
        mult *= 100;
    }
    return dec;
}

static void gb_ai_run_game(void)
{
    /* Start SUPER MARIOLAND game. */
    gb_ai_start_game();
    while (1) {
        if (glfwWindowShouldClose(GB.window)) {
            /* Close program. */
            gb_ai_finish();
            exit(EXIT_SUCCESS);
        }
        cpu_emulate_cycle();
        if (gb_ai.cpu->reg.pc == 0x03f4) {
            /* Game over: Loading start screen tiles. */
            uint8_t s[3];
            s[0] = mmu_read_byte(0xc0a0);
            s[1] = mmu_read_byte(0xc0a1);
            s[2] = mmu_read_byte(0xc0a2);
            unsigned int score = bcd_to_dec(s, 3);
            player_set_fitness(gb_ai.pl, score);
            printf("AI score: %d\n", score);
            break;
        }
    }
}

void gb_ai_main(void)
{
    while (1) {
        gb_ai_run_game();
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
