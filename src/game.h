#ifndef _GAME_h_
#define _GAME_h_

#include "memory.h"
#include "command.h"
#include "entity.h"

#include <SDL2/SDL_ttf.h>

#define PIXEL_PERSQUARE 64

struct GameState {
    bool init;

    struct Stack *game_stack;
    struct Stack *temp_stack;

    TTF_Font *font;

    struct Console *console;
};

#endif
