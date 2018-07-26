#include "text.h"

text_t *text_create(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color,
                    int x, int y, char *text)
{
    text_t *t = malloc(sizeof(text_t));
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (surface == NULL) {
        fprintf(stderr, "TTF_RenderText_Solid: %s\n", TTF_GetError());
        return NULL;
    }
    t->texture = SDL_CreateTextureFromSurface(renderer, surface);
    t->rect.w = surface->w;
    t->rect.h = surface->h;
    SDL_FreeSurface(surface);
    if (t->texture == NULL) {
        free(t);
        fprintf(stderr, "ERROR: %s\n", SDL_GetError());
        return NULL;
    }
    t->rect.x = x;
    t->rect.y = y;
    t->renderer = renderer;
    return t;
}

void text_destroy(text_t *t)
{
    SDL_DestroyTexture(t->texture);
    free(t);
}

void text_render(text_t *t)
{
    SDL_RenderCopy(t->renderer, t->texture, NULL, &t->rect);
}
