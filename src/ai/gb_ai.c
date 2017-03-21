#include "gb_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include "game_boy.h"
#include "keys.h"
#include "mmu.h"
#include "population.h"

#define NUM_PLAYERS (16)
/**
 * This file controls the game and AI initialization. For now it's hardcoded for
 * the Super Marioland game. In the future this should be configurable.
 */
typedef struct {
    population_t *pop;
    player_t *player;
    double *screen;
    unsigned int num_inputs;
    unsigned int num_outputs;
    cpu_t *cpu;
    gpu_t *gpu;
    unsigned int vblank_cnt;
    unsigned int last_vblank_score;
    unsigned int last_score;
    bool ai_enable;
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
    gb_ai.ai_enable = false;
    gb_ai.num_inputs = 160 * 144;
    gb_ai.num_outputs = 6;
    gb_ai.pop =
        population_create(NUM_PLAYERS, gb_ai.num_inputs, gb_ai.num_outputs);
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
    if (gb_ai.ai_enable == false)
        return;
    key_e key = KEY_B;
    for (unsigned int i = 0; i < num_outputs; ++i, ++key) {
        if (outputs[i] > 0.5) {
            /* Key pressed. */
            if (!key_check_pressed(key)) {
                printf("Pressing key %s\n", key_str(key));
                key_press(key);
            }
        } else {
            /* Key released. */
            if (key_check_pressed(key)) {
                printf("Releasing key %s\n", key_str(key));
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
        const double *keys = player_output(gb_ai.player, gb_ai.screen);
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
    gb_ai.vblank_cnt = 0;
    gb_ai.last_vblank_score = 0;
    gb_ai.last_score = 0;
    /* Execute until start game screen. */
    gb_ai_wait_vblank(2);
    /* Press start to start the game. */
    key_press(KEY_START);
    gb_ai_wait_vblank(1);
    key_release(KEY_START);
    gb_ai_wait_vblank(10);
    gb_ai.ai_enable = true;
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

static unsigned int gb_ai_get_score(void)
{
    /* Game over: Loading start screen tiles. */
    uint8_t s[3];
    s[0] = mmu_read_byte(0xc0a0);
    s[1] = mmu_read_byte(0xc0a1);
    s[2] = mmu_read_byte(0xc0a2);
    return bcd_to_dec(s, 3);
}

static void gb_ai_finish_game(void)
{
    unsigned int score = gb_ai_get_score();
    player_set_fitness(gb_ai.player, score);
    printf("AI score: %u\n", score);
    gb_ai.ai_enable = false;
}

static bool gb_ai_check_game_over(void)
{
    if (gb_ai.cpu->reg.pc == 0x03f4) {
        printf("Player game over detected\n");
        return true;
    } else if (gb_ai.cpu->reg.pc == 0x40) {
        gb_ai.vblank_cnt++;
        unsigned int cur_score = gb_ai_get_score();
        if (cur_score > gb_ai.last_score) {
            gb_ai.last_vblank_score = gb_ai.vblank_cnt;
            gb_ai.last_score = cur_score;
        } else if (gb_ai.last_vblank_score + 600 < gb_ai.vblank_cnt) {
            /* If after 10 seconds the score didn't change, timeout. */
            printf("Player timeout after 10 seconds without change in score\n");
            return true;
        }
    }
    return false;
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
        if (gb_ai_check_game_over() == true) {
            gb_ai_finish_game();
            cpu_reset();
            break;
        }
    }
}

void gb_ai_main(void)
{
    while (1) {
        /* Make every player play the game once. */
        for (unsigned int i = 0; i < NUM_PLAYERS; ++i) {
            printf("Current player: %u\n", i + 1);
            gb_ai.player = population_get_player(gb_ai.pop, i);
            gb_ai_run_game();
        }
        /* Select the best fitting players, and reproduce them. */
        population_natural_selection(gb_ai.pop);
    }
}

void gb_ai_finish(void)
{
    if (GB.window) {
        glfwDestroyWindow(GB.window);
        glfwTerminate();
    }
    population_destroy(gb_ai.pop);
}
