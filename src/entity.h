#ifndef _ENTITY_h_
#define _ENTITY_h_

#include "math.h"

struct ECS_Position {
    struct Vec2 diff;
    int x, y;
};

struct ECS_Movement {
    struct Vec2 vel;
};

struct ECS_Render {
    int frame;
    int dt;
};

struct ECS_Edible {
    int score;
    bool delete;
};

#define COMPONENT_BIND(_def) \
    _def(Position, 0)        \
    _def(Movement, 1)        \
    _def(Render,   2)        \
    _def(Edible,   3)

enum ECS_ComponentMask {
    ECS_CNone     = 0,
#define C_MASK(name, num) \
    ECS_C ## name = 1 << num,
    COMPONENT_BIND(C_MASK)
#undef C_MASK
};

struct ECS_Manager;
struct Stack;

struct ECS_Manager *ECS_NewManager(struct Stack *stack);

void ECS_AddComponent(struct ECS_Manager *manager, u32 eid, void *component, enum ECS_ComponentMask componentid);
void ECS_RemoveComponent(struct ECS_Manager *manager, u32 eid, enum ECS_ComponentMask componentid);

u32  ECS_AddEntity(struct ECS_Manager *manager);
void ECS_RemoveEntity(struct ECS_Manager *manager, u32 eid);
i64  ECS_NextEntity(struct ECS_Manager *manager, enum ECS_ComponentMask components);

#endif
