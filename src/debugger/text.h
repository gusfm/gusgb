#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Renderer *renderer;
    SDL_Rect rect;
    SDL_Texture *texture;
} text_t;

text_t *text_create(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color,
                    int x, int y, char *text);
void text_destroy(text_t *t);
void text_render(text_t *t);

#endif /* TEXT_H */
