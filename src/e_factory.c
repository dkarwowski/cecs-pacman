#include "entity.h"

/**
 * Create a new player
 *
 * @manager : ecs manager to add to
 *
 * @return : eid of the player
 */
u32
ECS_NewPlayer(struct ECS_Manager *manager)
{
    u32 result = ECS_AddEntity(manager);

    struct ECS_Position position = { 10, 14 };
    struct ECS_Movement movement = { (struct Vec2){ 0.f, 0.f }, (struct Vec2){ 0.f, 0.f } };
    struct ECS_Render   render   = { 1, 1, (SDL_Color){ 125, 125, 0, 255 } };
    struct ECS_Player   player   = { 3 };

    ECS_AddComponent(manager, result, &position, ECS_CPosition);
    ECS_AddComponent(manager, result, &movement, ECS_CMovement);
    ECS_AddComponent(manager, result, &render, ECS_CRender);
    ECS_AddComponent(manager, result, &player, ECS_CPlayer);

    return result;
}

/**
 * Create a new wall
 *
 * @manager : the ecs manager
 * @x, @y : where to spawn
 *
 * @return : eid of the dot
 */
u32
ECS_NewWall(struct ECS_Manager *manager, int x, int y)
{
    u32 result = ECS_AddEntity(manager);

    struct ECS_Position position = { x, y };
    struct ECS_Render   render   = { 1, 1, (SDL_Color){ 0, 125, 125, 255 } };

    ECS_AddComponent(manager, result, &position, ECS_CPosition);
    ECS_AddComponent(manager, result, &render, ECS_CRender);

    return result;
}

/**
 * Create a new dot
 *
 * @manager : the ecs manager
 * @x, @y : where to spawn
 *
 * @return : eid of the dot
 */
u32
ECS_NewDot(struct ECS_Manager *manager, int x, int y)
{
    u32 result = ECS_AddEntity(manager);

    struct ECS_Position position = { x, y };
    struct ECS_Render   render   = { 1, 1, (SDL_Color){ 125, 0, 125, 255 } };
    struct ECS_Edible   edible   = { 10, true };

    ECS_AddComponent(manager, result, &position, ECS_CPosition);
    ECS_AddComponent(manager, result, &render, ECS_CRender);
    ECS_AddComponent(manager, result, &edible, ECS_CEdible);

    return result;
}

/**
 * Create a new ghost
 *
 * @manager : the ecs manager
 * @x, @y : where to spawn
 * @color : ghost's color
 *
 * @return : eid of the dot
 */
u32
ECS_NewGhost(struct ECS_Manager *manager, int x, int y, SDL_Color color)
{
    u32 result = ECS_AddEntity(manager);

    struct ECS_Position position = { x, y };
    struct ECS_Movement movement = { (struct Vec2) { 0.f, 0.f }, (struct Vec2) { 0.f, 0.f} };
    struct ECS_Render   render   = { 1, 1, color };
    struct ECS_AI       ai       = { false };

    ECS_AddComponent(manager, result, &position, ECS_CPosition);
    ECS_AddComponent(manager, result, &movement, ECS_CMovement);
    ECS_AddComponent(manager, result, &render, ECS_CRender);
    ECS_AddComponent(manager, result, &ai, ECS_CAI);

    return result;
}