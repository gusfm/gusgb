#include "game_boy.h"
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "gpu.h"
#include "keys.h"

static void gb_key_press(game_boy_t *gb, uint8_t key)
{
    switch (key) {
        case SDL_SCANCODE_A:
            key_press(KEY_A);
            break;
        case SDL_SCANCODE_S:
            key_press(KEY_B);
            break;
        case SDL_SCANCODE_RETURN:
            key_press(KEY_START);
            break;
        case SDL_SCANCODE_LSHIFT:
            key_press(KEY_SELECT);
            break;
        case SDL_SCANCODE_UP:
            key_press(KEY_UP);
            break;
        case SDL_SCANCODE_DOWN:
            key_press(KEY_DOWN);
            break;
        case SDL_SCANCODE_LEFT:
            key_press(KEY_LEFT);
            break;
        case SDL_SCANCODE_RIGHT:
            key_press(KEY_RIGHT);
            break;
        case SDL_SCANCODE_P:
            /* Pause emulation. */
            gb->paused = !gb->paused;
            break;
        case SDL_SCANCODE_O:
            /* Debug CPU. */
            cpu_dump();
            break;
        case SDL_SCANCODE_Q:
        case SDL_SCANCODE_ESCAPE:
            /* Quit. */
            gb->running = false;
            break;
        default:
            break;
    }
}

static void gb_key_release(uint8_t key)
{
    switch (key) {
        case SDL_SCANCODE_A:
            key_release(KEY_A);
            break;
        case SDL_SCANCODE_S:
            key_release(KEY_B);
            break;
        case SDL_SCANCODE_RETURN:
            key_release(KEY_START);
            break;
        case SDL_SCANCODE_LSHIFT:
            key_release(KEY_SELECT);
            break;
        case SDL_SCANCODE_UP:
            key_release(KEY_UP);
            break;
        case SDL_SCANCODE_DOWN:
            key_release(KEY_DOWN);
            break;
        case SDL_SCANCODE_LEFT:
            key_release(KEY_LEFT);
            break;
        case SDL_SCANCODE_RIGHT:
            key_release(KEY_RIGHT);
            break;
        default:
            break;
    }
}

static SDL_Window *sdl_init(const char *name, int width, int height)
{
    /* Initialize SDL. */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return NULL;
    return SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, width, height,
                            SDL_WINDOW_SHOWN);
}

int gb_init(game_boy_t *gb, int scale, const char *rom_path)
{
    gb->width = GB_SCREEN_WIDTH * scale;
    gb->height = GB_SCREEN_HEIGHT * scale;
    gb->running = true;
    gb->paused = false;
    /* Initialize SDL. */
    gb->window = sdl_init("gusgb", gb->width, gb->height);
    if (gb->window == NULL) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return -1;
    }
    /* Initialize emulation. */
    if (cpu_init(rom_path) < 0) {
        fprintf(stderr, "ERROR: Could not load rom: %s\n", rom_path);
        return -1;
    }
    if (gpu_init(gb->window) < 0) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
    }
    return 0;
}

void gb_finish(game_boy_t *gb)
{
    gpu_finish();
    cpu_finish();
    SDL_DestroyWindow(gb->window);
    SDL_Quit();
}

void gb_main(game_boy_t *gb)
{
    SDL_Event e;
    while (gb->running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_KEYDOWN:
                    gb_key_press(gb, e.key.keysym.scancode);
                    break;
                case SDL_KEYUP:
                    gb_key_release(e.key.keysym.scancode);
                    break;
                case SDL_QUIT:
                    gb->running = false;
                    break;
            }
        }
        if (gb->paused) {
            gpu_render_framebuffer();
        } else {
            cpu_emulate_cycle();
        }
    }
}
