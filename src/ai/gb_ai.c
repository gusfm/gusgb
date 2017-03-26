#include "gb_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include "game_boy.h"
#include "keys.h"
#include "mmu.h"
#include "population.h"

#define NUM_PLAYERS (32)
#define GAME_TIMEOUT (5)
#define GAME_TIMEOUT_FRAMES (60 * GAME_TIMEOUT)

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
} gb_ai_t;

unsigned int vblank_cnt;
unsigned int last_score;
unsigned int last_score_frame;
unsigned int key_score;
unsigned int frame_cnt;
unsigned int epoch = 0;
uint16_t *pc;
rgb_t *fb;
bool game_over;

gb_ai_t gb_ai;

int gb_ai_init(int width, int height, float window_zoom, const char *rom_path)
{
    int ret = gb_init(width, height, window_zoom, rom_path);
    if (ret != 0)
        return ret;
    /* Get instance of CPU and GPU. */
    gb_ai.cpu = cpu_get_instance();
    gb_ai.gpu = gpu_get_instance();
    pc = &gb_ai.cpu->reg.pc;
    fb = gb_ai.gpu->framebuffer;
    /* Disable gl rendering until the end of intro logo. */
    gpu_gl_disable();
    gb_ai.num_inputs = 80 * 72; /* 1/4 of original size. */
    gb_ai.num_outputs = 6;
    gb_ai.pop =
        population_create(NUM_PLAYERS, gb_ai.num_inputs, gb_ai.num_outputs);
    gb_ai.screen = (double *)malloc(sizeof(double) * gb_ai.num_inputs);
    return 0;
}

int gb_ai_load(const char *filename)
{
    return population_load(gb_ai.pop, filename, &epoch);
}

static void get_input(double *inputs)
{
    /* Resize image to reduce the number of inputs. */
    unsigned int i = 0, px, offs1, offs2;
    for (unsigned int y = 0; y < 144; y += 2) {
        offs1 = y * 160;
        offs2 = (y + 1) * 160;
        for (unsigned int x = 0; x < 160; x += 2) {
            px = fb[offs1++].r;
            px += fb[offs1++].r;
            px += fb[offs2++].r;
            px += fb[offs2++].r;
            inputs[i++] = (double) px / (255.0 * 4.0);
        }
    }
}

static void check_player_keys(const double *outputs, unsigned int num_outputs)
{
    key_e key = KEY_B;
    for (unsigned int i = 0; i < num_outputs; ++i, ++key) {
        if (outputs[i] > 0.5) {
            if (!key_check_pressed(key)) {
                /* Key pressed. */
                key_press(key);
                if (key_score < 200) {
                    if (key == KEY_A) {
                        key_score += 10;
                    } else if (key == KEY_RIGHT) {
                        key_score += 5;
                    }
                }
            }
        } else {
            if (key_check_pressed(key)) {
                /* Key released. */
                key_release(key);
            }
        }
    }
}

static void gb_ai_wait_frames(unsigned int num)
{
    frame_cnt = 0;
    do {
        cpu_emulate_cycle();
    } while (frame_cnt < num);
}

static void start_game_step_cb(void)
{
    ++frame_cnt;
}

static void gb_ai_start_game(void)
{
    frame_cnt = 0;
    vblank_cnt = 0;
    last_score = 0;
    last_score_frame = 0;
    key_score = 0;
    game_over = false;
    /* Execute until start game screen. */
    gpu_set_callback(start_game_step_cb);
    gb_ai_wait_frames(10);
    /* Press start to start the game. */
    key_press(KEY_START);
    gb_ai_wait_frames(10);
    key_release(KEY_START);
    gb_ai_wait_frames(10);
    gpu_set_callback(gb_ai_step);
    frame_cnt = 0;
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

void gb_ai_step(void)
{
    if (++frame_cnt & 1) {
        /* Only calculate output every odd frame. */
        get_input(gb_ai.screen);
        const double *keys = player_output(gb_ai.player, gb_ai.screen);
        check_player_keys(keys, gb_ai.num_outputs);
        /* Check game over. */
        unsigned int cur_score = gb_ai_get_score();
        if (cur_score > last_score) {
            last_score = cur_score;
            last_score_frame = frame_cnt;
        } else if (frame_cnt > last_score_frame + GAME_TIMEOUT_FRAMES) {
            /* If after GAME_TIMEOUT seconds the score didn't change, timeout. */
            printf("Player timeout after %u seconds without change in score\n", GAME_TIMEOUT);
            game_over = true;
        }
    }
}

static void gb_ai_finish_game(void)
{
    unsigned int score = gb_ai_get_score() + key_score;
    player_set_fitness(gb_ai.player, score);
    printf("AI score: %u\n", score);
}

static void gb_ai_run_game(void)
{
    /* Start SUPER MARIOLAND game. */
    gb_ai_start_game();
    while (!game_over) {
        cpu_emulate_cycle();
    }
    gb_ai_finish_game();
    cpu_reset();
    if (glfwWindowShouldClose(GB.window)) {
        /* Close program. */
        gb_ai_finish();
        exit(EXIT_SUCCESS);
    }
}

void gb_ai_main(void)
{
    while (1) {
        printf("Epoch: %u\n", epoch);
        /* Make every player play the game once. */
        for (unsigned int i = 0; i < NUM_PLAYERS; ++i) {
            printf("Current player: %u\n", i + 1);
            gb_ai.player = population_get_player(gb_ai.pop, i);
            gb_ai_run_game();
        }
        /* Select the best fitting players, and reproduce them. */
        population_natural_selection(gb_ai.pop);
        ++epoch;
    }
}

void gb_ai_finish(void)
{
    if (GB.window) {
        glfwDestroyWindow(GB.window);
        glfwTerminate();
    }
    population_save(gb_ai.pop, "population.out", epoch);
    population_destroy(gb_ai.pop);
}
