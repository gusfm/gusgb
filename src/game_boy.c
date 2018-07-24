#include "game_boy.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "apu/apu.h"
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
    SDL_Renderer *ren;
    SDL_Texture *tex;
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

static int sdl_init(const char *name, int width, int height, bool fullscreen)
{
    SDL_AudioSpec desired;
    uint32_t flags = 0;

    /* Initialize SDL. */
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
        return -1;

    /* Initialize main window. */
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        SDL_ShowCursor(SDL_DISABLE);
    }
    GB.window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    if (GB.window == NULL) {
        return -1;
    }

    /* Create SDL renderer */
    GB.ren = SDL_CreateRenderer(
        GB.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (GB.ren == NULL)
        return -1;

    /* Set device independent resolution for rendering */
    SDL_RenderSetLogicalSize(GB.ren, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT);
    /* Create SDL texture */
    GB.tex = SDL_CreateTexture(GB.ren, SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING, GB_SCREEN_WIDTH,
                               GB_SCREEN_HEIGHT);
    if (GB.tex == NULL)
        return -1;

    /* Initialized audio. */
    desired.freq = AUDIO_SAMPLE_RATE;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = AUDIO_SAMPLE_SIZE;
    desired.callback = NULL;
    if (SDL_OpenAudio(&desired, NULL) != 0) {
        return -1;
    }
    SDL_PauseAudio(0);
    return 0;
}

int gb_init(int scale, const char *rom_path, bool fullscreen)
{
    GB.width = GB_SCREEN_WIDTH * scale;
    GB.height = GB_SCREEN_HEIGHT * scale;
    GB.running = true;
    GB.paused = false;
    GB.fullscreen = fullscreen;
    /* Initialize SDL. */
    if (sdl_init("gusgb", GB.width, GB.height, GB.fullscreen) != 0) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return -1;
    }
    /* Initialize emulation. */
    if (cpu_init(rom_path) < 0) {
        fprintf(stderr, "ERROR: Could not load rom: %s\n", rom_path);
        return -1;
    }
    if (gpu_init(GB.ren, GB.tex, handle_events) < 0) {
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
    }
    return 0;
}

void gb_finish(void)
{
    cpu_finish();
    SDL_PauseAudio(1);
    SDL_DestroyTexture(GB.tex);
    SDL_DestroyRenderer(GB.ren);
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
