#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "../color.h"

typedef struct {
    int x;
    int y;
    int w;
    int h;
    bool pressed;
    color_t color;
} button_t;

void button_render(SDL_Renderer *renderer, button_t *b);

#endif /* BUTTON_H */
