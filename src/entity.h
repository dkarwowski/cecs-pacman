#ifndef _ENTITY_h_
#define _ENTITY_h_

#include "math.h"
#include <SDL/SDL.h>

struct ECS_Position {
    struct Vec2 pos;
};

struct ECS_Bounding {
    bool stop;
    struct Vec2 dim;
};

struct ECS_Movement {
    struct Vec2 vel;
    r32 speed;
};

struct ECS_Render {
    r32 w, h;
    SDL_Color color;
};

struct ECS_Edible {
    u32 score;
    bool delete;
};

struct ECS_Player {
    u8 lives;
    u32 score;
};

struct ECS_AI {
    bool run_away;
};

struct ECS_Collided {
    u32 with;
    struct Vec2 normal;
};

#define COMPONENT_BIND(_def) \
    _def(Position, 0)        \
    _def(Movement, 1)        \
    _def(Render,   2)        \
    _def(Edible,   3)        \
    _def(AI,       4)        \
    _def(Player,   5)        \
    _def(Collided, 6)        \
    _def(Bounding, 7)        \

enum ECS_ComponentMask {
    ECS_CNone     = 0,
#define C_MASK(name, num) \
    ECS_C ## name = 1 << num,
    COMPONENT_BIND(C_MASK)
#undef C_MASK
};

struct ECS_Iter {
    enum ECS_ComponentMask mask;
    u32 eid;
};

struct ECS_Manager;
struct Stack;

struct ECS_Manager *ECS_NewManager(struct Stack *stack);

void  ECS_AddComponent(struct ECS_Manager *manager, u32 eid, void *component, enum ECS_ComponentMask componentid);
void  ECS_RemoveComponent(struct ECS_Manager *manager, u32 eid, enum ECS_ComponentMask componentid);
void *ECS_GetComponent(struct ECS_Manager *manager, u32 eid, enum ECS_ComponentMask componentid);
bool  ECS_HasComponent(struct ECS_Manager *manager, u32 eid, enum ECS_ComponentMask componentid);

u32  ECS_AddEntity(struct ECS_Manager *manager);
void ECS_RemoveEntity(struct ECS_Manager *manager, u32 eid);
i64  ECS_NextEntity(struct ECS_Manager *manager, struct ECS_Iter *iter);

#endif
