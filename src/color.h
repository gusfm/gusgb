#ifndef COLOR_H
#define COLOR_H

#include <SDL2/SDL.h>

typedef struct {
#if (SDL_BYTE_ORDER == SDL_BIG_ENDIAN)
    uint8_t a, r, g, b;
#else
    uint8_t b, g, r, a;
#endif
} color_t;

#endif /* COLOR_H */
