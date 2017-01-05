#include "entity.h"

/**
 * Create a new player
 *
 * @manager : ecs manager to add to
 *
 * @return : eid of the player
 */
u32
ECS_NewPlayer(struct ECS_Manager *manager, int x, int y)
{
    u32 result = ECS_AddEntity(manager);

    struct ECS_Position position = { (struct Vec2){ x, y } };
    struct ECS_Bounding bounding = { false, (struct Vec2){ 0.45f, 0.45f } };
    struct ECS_Movement movement = { (struct Vec2){ 0.f, 0.f }, 3.5f };
    struct ECS_Render   render   = { 0.45f, 0.45f, (SDL_Color){ 255, 255, 0, 255 } };
    struct ECS_Player   player   = { 3, 0 };
    struct ECS_Respawn  respawn  = { (struct Vec2){ x, y }, 3.f, 0.f };

    ECS_AddComponent(manager, result, &position, ECS_CPosition);
    ECS_AddComponent(manager, result, &bounding, ECS_CBounding);
    ECS_AddComponent(manager, result, &movement, ECS_CMovement);
    ECS_AddComponent(manager, result, &render, ECS_CRender);
    ECS_AddComponent(manager, result, &player, ECS_CPlayer);
    ECS_AddComponent(manager, result, &respawn, ECS_CRespawn);

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

    struct ECS_Position position = { (struct Vec2){ x, y } };
    struct ECS_Bounding bounding = { true, (struct Vec2){ 0.5f, 0.5f } };
    struct ECS_Render   render   = { 0.5f, 0.5f, (SDL_Color){ 0, 125, 125, 255 } };

    ECS_AddComponent(manager, result, &position, ECS_CPosition);
    ECS_AddComponent(manager, result, &bounding, ECS_CBounding);
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

    struct ECS_Position position = { (struct Vec2) { x, y } };
    struct ECS_Bounding bounding = { false, (struct Vec2){ 0.25f, 0.25f } };
    struct ECS_Render   render   = { 0.25f, 0.25f, (SDL_Color){ 125, 0, 125, 255 } };
    struct ECS_Edible   edible   = { 10, true };

    ECS_AddComponent(manager, result, &position, ECS_CPosition);
    ECS_AddComponent(manager, result, &bounding, ECS_CBounding);
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

    struct ECS_Position position = { (struct Vec2){ x, y } };
    struct ECS_Bounding bounding = { true, (struct Vec2){ 0.45f, 0.45f } };
    struct ECS_Movement movement = { (struct Vec2) { 0.f, 0.f }, 1.f };
    struct ECS_Render   render   = { 0.45f, 0.45f, color };
    struct ECS_AI       ai       = { false, true, (struct Vec2){ 0.f, 0.f }, (struct Vec2){ 0.f, 0.f } };
    struct ECS_Respawn  respawn  = { (struct Vec2){ x, y }, 2.f, 0.f };

    ECS_AddComponent(manager, result, &position, ECS_CPosition);
    ECS_AddComponent(manager, result, &bounding, ECS_CBounding);
    ECS_AddComponent(manager, result, &movement, ECS_CMovement);
    ECS_AddComponent(manager, result, &render, ECS_CRender);
    ECS_AddComponent(manager, result, &ai, ECS_CAI);
    ECS_AddComponent(manager, result, &respawn, ECS_CRespawn);

    return result;
}
