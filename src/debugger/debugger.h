#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <SDL2/SDL.h>

int debugger_init(void);
void debugger_finish(void);
void debugger_render(void);
void debugger_handle_events(SDL_Event *e);

#endif /* DEBUGGER_H */
