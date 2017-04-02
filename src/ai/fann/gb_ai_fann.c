#include "gb_ai_fann.h"
#include <assert.h>
#include <doublefann.h>
#include <fann.h>
#include <stdio.h>
#include <stdlib.h>
#include "ai.h"
#include "game_boy.h"
#include "keys.h"

/**
 * This file controls the game and AI initialization. For now it's hardcoded for
 * the Super Marioland game. In the future this should be configurable.
 */

double *inputs;
rgb_t *fb;
unsigned int frame_cnt;
struct fann *ann;

int gb_ai_init(int width, int height, float window_zoom, const char *rom_path)
{
    int ret = gb_init(width, height, window_zoom, rom_path);
    if (ret != 0)
        return ret;
    /* Get frame buffer. */
    fb = gpu_get_instance()->framebuffer;
    /* Disable gl rendering until the end of intro logo. */
    gpu_gl_disable();
    return 0;
}

static void get_screen(uint8_t *screen)
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
            screen[i++] = (uint8_t)(px / 4u);
        }
    }
}

static void get_screen_norm(double *screen)
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
            screen[i++] = (double)px / (255.0 * 4.0);
        }
    }
}

static void check_player_keys(const double *outputs)
{
    key_e key = KEY_B;
    for (unsigned int i = 0; i < NUM_OUTPUTS; ++i, ++key) {
        printf("%s:%d: outputs[%u]=%lf\n", __func__, __LINE__, i, outputs[i]);
        if (outputs[i] > 0.5) {
            if (!key_check_pressed(key)) {
                /* Key pressed. */
                printf("%s:%d: key %d pressed\n", __func__, __LINE__, key);
                key_press(key);
            }
        } else {
            if (key_check_pressed(key)) {
                /* Key released. */
                printf("%s:%d: key %d released\n", __func__, __LINE__, key);
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

static void gb_ai_step(void)
{
    if (++frame_cnt & 1) {
        /* Only calculate output every odd frame. */
        get_screen_norm(inputs);
        fann_type *keys = fann_run(ann, inputs);
        check_player_keys(keys);
    }
}

static void gb_ai_start_game(void)
{
    frame_cnt = 0;
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

void gb_ai_main(const char *nn_path)
{
    /* Load fann nn. */
    inputs = (double *)malloc(sizeof(double) * NUM_INPUTS);
    ann = fann_create_from_file(nn_path);
    /* Start SUPER MARIOLAND game. */
    gb_ai_start_game();
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
    if (inputs) {
        free(inputs);
    }
}

FILE *training_file = NULL;
unsigned int num_data = 0;

static void gb_ai_tr_write_inputs(void)
{
    uint8_t screen[NUM_INPUTS];
    get_screen(screen);
    size_t ret = fwrite(screen, 1, sizeof(screen), training_file);
    assert(ret == NUM_INPUTS);
}

static void gb_ai_tr_write_outputs(void)
{
    key_e key = KEY_B;
    uint8_t outputs = 0;
    for (unsigned int i = 0; i < NUM_OUTPUTS; ++i, ++key) {
        uint8_t val;
        if (key_check_pressed(key)) {
            val = 1;
        } else {
            val = 0;
        }
        outputs = (uint8_t)(outputs | (val << i));
    }
    size_t ret = fwrite(&outputs, 1, 1, training_file);
    assert(ret == 1);
}

static void gb_ai_tr_step(void)
{
    if (++frame_cnt & 1) {
        gb_ai_tr_write_inputs();
        gb_ai_tr_write_outputs();
        ++num_data;
    }
}

void gb_ai_tr_main(const char *tr_path)
{
    /* Wait 2 seconds before start to recording the training file. */
    gpu_set_callback(start_game_step_cb);
    gb_ai_wait_frames(120);
    frame_cnt = 0;
    training_file = fopen(tr_path, "w");
    assert(training_file != NULL);
    /* Save space for number of data. */
    if (fwrite(&num_data, sizeof(num_data), 1, training_file) != 1) {
        fprintf(stderr, "ERROR: could not write num_data %u\n", num_data);
    }
    printf("Starting to record training file %s\n", tr_path);
    gpu_set_callback(gb_ai_tr_step);
    gb_main();
    while (!glfwWindowShouldClose(GB.window)) {
        cpu_emulate_cycle();
    }
    /* Write correct number of data written. */
    rewind(training_file);
    if (fwrite(&num_data, sizeof(num_data), 1, training_file) != 1) {
        fprintf(stderr, "ERROR: could not write num_data %u\n", num_data);
    }
    printf("File %s written with %u training data\n", tr_path, num_data);
    fclose(training_file);
}
