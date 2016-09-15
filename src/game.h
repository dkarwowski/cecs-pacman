#ifndef _GAME_h_
#define _GAME_h_

#include "memory.h"
#include "command.h"
#include "entity.h"

#include <SDL2/SDL_ttf.h>

#define PIXEL_PERSQUARE (32)

struct CollisionNode {
    u32 one, two;
    struct CollisionNode *next;
};

struct CollisionList {
    struct Stack *stack;
    struct CollisionNode *first;
};

struct GameState {
    bool init;

    struct Stack *game_stack;
    struct Stack *temp_stack;

    TTF_Font *font;

    struct ECS_Manager *manager;
    u32 player_eid;

    u8 map[21][19];

    struct CollisionList collides;

    struct Console *console;
};

#endif
