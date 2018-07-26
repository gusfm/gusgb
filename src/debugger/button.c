#include "button.h"

void button_render_pressed(SDL_Renderer *renderer, button_t *b)
{
    SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, b->x, b->y, b->x, b->y + b->h - 1);
    SDL_RenderDrawLine(renderer, b->x, b->y, b->x + b->w - 1, b->y);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, b->x, b->y + b->h, b->x + b->w, b->y + b->h);
    SDL_RenderDrawLine(renderer, b->x + b->w, b->y + b->h, b->x + b->w, b->y);
    SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, b->x + 1, b->y + 1, b->x + 1, b->y + b->h - 2);
    SDL_RenderDrawLine(renderer, b->x + 1, b->y + 1, b->x + b->w - 2, b->y + 1);
}

void button_render_released(SDL_Renderer *renderer, button_t *b)
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, b->x, b->y, b->x, b->y + b->h - 1);
    SDL_RenderDrawLine(renderer, b->x, b->y, b->x + b->w - 1, b->y);
    SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, b->x, b->y + b->h, b->x + b->w, b->y + b->h);
    SDL_RenderDrawLine(renderer, b->x + b->w, b->y + b->h, b->x + b->w, b->y);
    SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, b->x + 1, b->y + b->h - 1, b->x + b->w - 1,
                       b->y + b->h - 1);
    SDL_RenderDrawLine(renderer, b->x + b->w - 1, b->y + b->h - 1,
                       b->x + b->w - 1, b->y + 1);
}

void button_render(SDL_Renderer *renderer, button_t *b)
{
    SDL_Rect rect = {b->x + 2, b->y + 2, b->w - 3, b->h - 3};
    SDL_SetRenderDrawColor(renderer, b->color.r, b->color.g, b->color.b,
                           SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
    if (b->pressed)
        button_render_pressed(renderer, b);
    else
        button_render_released(renderer, b);
}
