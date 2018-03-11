#include "game_boy.h"
#include <stdio.h>
#include <stdlib.h>
#include "apu.h"
#include "cpu.h"
#include "gpu.h"
#include "keys.h"

typedef struct {
    int width;
    int height;
    bool running;
    bool paused;
    bool fullscreen;
    SDL_Window *window;
} game_boy_t;

static game_boy_t GB;

static void gb_toggle_fullscreen(void)
{
    GB.fullscreen = !GB.fullscreen;
    if (GB.fullscreen) {
        SDL_SetWindowFullscreen(GB.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(SDL_DISABLE);
    } else {
        SDL_SetWindowFullscreen(GB.window, 0);
        SDL_ShowCursor(SDL_ENABLE);
    }
}

static void gb_key_press(const SDL_Keysym *keysym)
{
    switch (keysym->scancode) {
        case SDL_SCANCODE_A:
            key_press(KEY_A);
            break;
        case SDL_SCANCODE_S:
            key_press(KEY_B);
            break;
        case SDL_SCANCODE_RETURN:
            if (keysym->mod & KMOD_ALT) {
                gb_toggle_fullscreen();
            } else {
                key_press(KEY_START);
            }
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
            GB.paused = !GB.paused;
            break;
        case SDL_SCANCODE_O:
            /* Debug CPU. */
            cpu_dump();
            break;
        case SDL_SCANCODE_Q:
        case SDL_SCANCODE_ESCAPE:
            /* Quit. */
            GB.running = false;
            break;
        default:
            break;
    }
}

static void gb_key_release(const SDL_Keysym *keysym)
{
    switch (keysym->scancode) {
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

static void handle_events(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_KEYDOWN:
                gb_key_press(&e.key.keysym);
                break;
            case SDL_KEYUP:
                gb_key_release(&e.key.keysym);
                break;
            case SDL_QUIT:
                GB.running = false;
                break;
        }
    }
}

static SDL_Window *sdl_init(const char *name, int width, int height,
                            bool fullscreen)
{
    /* Initialize SDL. */
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
        return NULL;
    SDL_AudioSpec desired;
    desired.freq = AUDIO_SAMPLE_RATE;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 1024;
    desired.callback = apu_sdl_cb;
    if (SDL_OpenAudio(&desired, NULL) != 0) {
        return NULL;
    }
    uint32_t flags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        SDL_ShowCursor(SDL_DISABLE);
    }
    return SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, width, height, flags);
}

int gb_init(int scale, const char *rom_path, bool fullscreen)
{
    GB.width = GB_SCREEN_WIDTH * scale;
    GB.height = GB_SCREEN_HEIGHT * scale;
    GB.running = true;
    GB.paused = false;
    GB.fullscreen = fullscreen;
    /* Initialize SDL. */
    GB.window = sdl_init("gusgb", GB.width, GB.height, GB.fullscreen);
    if (GB.window == NULL) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return -1;
    }
    /* Initialize emulation. */
    if (cpu_init(rom_path) < 0) {
        fprintf(stderr, "ERROR: Could not load rom: %s\n", rom_path);
        return -1;
    }
    if (gpu_init(GB.window, handle_events) < 0) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
    }
    return 0;
}

void gb_finish(void)
{
    gpu_finish();
    cpu_finish();
    SDL_DestroyWindow(GB.window);
    SDL_Quit();
}

void gb_main(void)
{
    while (GB.running) {
        if (GB.paused) {
            gpu_render_framebuffer();
        } else {
            cpu_emulate_cycle();
        }
    }
}
