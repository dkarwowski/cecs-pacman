#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#include "main.h"
#include "math.h"
#include "e_factory.h"

#include "game.h"

static
u8 init_map[21][19] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
                        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                        { 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1 },
                        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                        { 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1 },
                        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
                        { 1, 1, 1, 1, 0, 1, 1, 1, 9, 1, 9, 1, 1, 1, 0, 1, 1, 1, 1 },
                        { 9, 9, 9, 1, 0, 1, 9, 9, 9, 2, 9, 9, 9, 1, 0, 1, 9, 9, 9 },
                        { 1, 1, 1, 1, 0, 1, 9, 1, 1, 8, 1, 1, 9, 1, 0, 1, 1, 1, 1 },
                        { 9, 9, 9, 9, 0, 9, 9, 1, 3, 4, 5, 1, 9, 9, 0, 9, 9, 9, 9 },
                        { 1, 1, 1, 1, 0, 1, 9, 1, 1, 1, 1, 1, 9, 1, 0, 1, 1, 1, 1 },
                        { 9, 9, 9, 1, 0, 1, 9, 9, 9, 9, 9, 9, 9, 1, 0, 1, 9, 9, 9 },
                        { 1, 1, 1, 1, 0, 1, 9, 1, 1, 1, 1, 1, 9, 1, 0, 1, 1, 1, 1 },
                        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                        { 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1 },
                        { 1, 0, 0, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
                        { 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1 },
                        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
                        { 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1 },
                        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

void
HandleCollides( struct ECS_Manager *manager,
                u32 eid,
                struct ECS_Position *pos,
                struct ECS_Movement *move,
                struct ECS_Bounding *box )
{
    struct Vec2 dpos = V2_Mul(SEC_PER_UPDATE, move->vel);
    bool stop = false;

    r32 tleft = 1.0f;
    for (int z = 0; z < 4 && tleft > 0.0f; z++) {
        struct Vec2 normal = { 0.f, 0.f };
        r32 tmin = 1.0f;
        
        u32 ieid;
        struct ECS_Iter iter = { ECS_CBounding | ECS_CPosition, 0 };
        while ((ieid = ECS_NextEntity(manager, &iter)) != -1) {
            if (ieid == eid)
                continue;

            struct ECS_Position *ipos = ECS_GetComponent(manager, ieid, ECS_CPosition);
            struct ECS_Bounding *ibox = ECS_GetComponent(manager, ieid, ECS_CBounding);

            r32 points[] = { ipos->pos.x - ibox->dim.w - box->dim.w - pos->pos.x,
                               ipos->pos.y - ibox->dim.h - box->dim.h - pos->pos.y,
                               ipos->pos.x + ibox->dim.w + box->dim.w - pos->pos.x,
                               ipos->pos.y + ibox->dim.h + box->dim.h - pos->pos.y };

            struct {
                r32 x0, x1, y, dy, dx;
                struct Vec2 normal;
            } walls[] = {{ points[0], points[2], points[1], dpos.y, dpos.x, {  0.f, -1.f } },
                         { points[0], points[2], points[3], dpos.y, dpos.x, {  0.f,  1.f } },
                         { points[1], points[3], points[0], dpos.x, dpos.y, { -1.f,  0.f } },
                         { points[1], points[3], points[2], dpos.x, dpos.y, {  1.f,  0.f } }};

            for (int walli = 0; walli < 4; walli++) {
                r32 eps = 0.001f;
                if (fabsf(walls[walli].dy) > eps) {
                    r32 t = walls[walli].y / walls[walli].dy;
                    r32 x = t * walls[walli].dx;
                    if (t > 0.0f && walls[walli].x0 < x && x < walls[walli].x1 && tmin > t) {
                        if (ibox->stop) {
                            tmin = MAX(0.f, t - eps);
                            normal = walls[walli].normal;
                            stop = true;
                        }
                        struct ECS_Collided col = { ieid };
                        ECS_AddComponent(manager, eid, &col, ECS_CCollided);
                    }
                }
            }
        }

        if (stop) {
            pos->pos = V2_Add(pos->pos, V2_Mul(tmin, dpos));
            move->vel = V2_Sub(move->vel, V2_Mul(V2_Dot(move->vel, normal), normal));
            dpos = V2_Sub(dpos, V2_Mul(V2_Dot(dpos, normal), normal));
            tleft -= tmin;
        }
        else {
            pos->pos = V2_Add(pos->pos, dpos);
            break;
        }
    }

    if (dpos.x > 0.01f && fabsf(dpos.y) < 0.01f)
        pos->pos.y = (r32)((int)(pos->pos.y + 0.5f));
    if (dpos.y > 0.01f && fabsf(dpos.x) < 0.01f)
        pos->pos.x = (r32)((int)(pos->pos.x + 0.5f));
}

/**
 * Update the game state and objects
 * @memory : the memory we keep constant
 * @input  : input from the main
 *
 * This can run many times before rendering, or just once. Requires some
 * MS_PER_UPDATE to be defined.
 */
extern
UPDATE(Update) /* memory, input */
{
    struct GameState *state = (struct GameState *)memory->perm_mem;

    if (!state->init) {
        state->init = true;

        state->game_stack = Z_NewStack( memory->perm_mem + sizeof(struct GameState),
                                        memory->perm_memsize - sizeof(struct GameState) );
        state->temp_stack = Z_NewStack( memory->temp_mem,
                                        memory->temp_memsize);

        state->console = I_NewConsole(state->game_stack, input);

        memcpy(state->map, init_map, sizeof(init_map));

        /* Initialize font as best possible, if it fails then ensure it's NULL */
        if (!TTF_WasInit()) {
            if (TTF_Init() == -1) {
                state->font = NULL;
                fprintf(stderr, "Can't initialize TTF\n");
            } else {
                state->font = TTF_OpenFont("../res/VeraMono.ttf", 12);
                if (state->font == NULL)
                    fprintf(stderr, "Can't open the font: %s\n", TTF_GetError());
            }
        }

        /* TODO(david): initialize the game state here */
        state->manager = ECS_NewManager(state->game_stack);

        for (int r = 0; r < 21; r++) {
            for (int c = 0; c < 19; c++) {
                u8 val = state->map[r][c];
                switch (val) {
                    case 0:
                        ECS_NewDot(state->manager, c, r);
                        break;
                    case 1:
                        ECS_NewWall(state->manager, c, r);
                        break;
                    case 2:
                        ECS_NewGhost(state->manager, c, r, (SDL_Color){255, 0, 0, 255});
                        break;
                    case 3:
                        ECS_NewGhost(state->manager, c, r, (SDL_Color){0, 255, 0, 255});
                        break;
                    case 4:
                        ECS_NewGhost(state->manager, c, r, (SDL_Color){0, 0, 255, 255});
                        break;
                    case 5:
                        ECS_NewGhost(state->manager, c, r, (SDL_Color){0, 255, 255, 255});
                        break;
                    case 7:
                        state->player_eid = ECS_NewPlayer(state->manager, c, r);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    /* Handle Input ------------------------------------------------------- */
    I_HandleCommand(state, input);

    /* handle everything for quitting out immediately */
    if (I_IsPressed(&input->quit)) {
        TTF_CloseFont(state->font);
        state->font = NULL;
        TTF_Quit();
        return;
    }

    /* toggle whether we're using the console or not */
    if (I_IsToggled(&input->console))
        I_ToggleConsole(state->console);

    /* we don't want to actually do any of the real updating if the console
     * is currently running. This allows for us to make changes and then,
     * in a sense, resume the game with those changes in place */
    if (I_IsEnabled(state->console))
        return;

    /* TODO(david): insert game code here */

    /* INPUT SYSTEM */
    struct ECS_Iter iter = { ECS_CPlayer | ECS_CMovement, 0 };
    i64 eid;
    while ((eid = ECS_NextEntity(state->manager, &iter)) != -1) {
        struct ECS_Movement *move = ECS_GetComponent(state->manager, eid, ECS_CMovement);

        struct Vec2 new_vel = { 0.f, 0.f };
        if (I_IsPressed(&input->move_left))
            new_vel.x -= move->speed;
        if (I_IsPressed(&input->move_right))
            new_vel.x += move->speed;
        if (I_IsPressed(&input->move_up))
            new_vel.y -= move->speed;
        if (I_IsPressed(&input->move_down))
            new_vel.y += move->speed;
        move->vel = new_vel;
    }

//    struct ECS_Position *player_pos = ECS_GetComponent(state->manager, eid, ECS_CPosition);
//    iter = (struct ECS_Iter){ ECS_CAI | ECS_CMovement, 0 };
//    while ((eid = ECS_NextEntity(state->manager, &iter)) != -1) {
//        struct ECS_Movement *move = ECS_GetComponent(state->manager, eid, ECS_CMovement);
//        struct ECS_AI       *ai   = ECS_GetComponent(state->manager, eid, ECS_CAI);
//
//        /* determine next move */
//    }

    /* movement system */
    iter = (struct ECS_Iter){ ECS_CPosition | ECS_CMovement, 0 };
    while ((eid = ECS_NextEntity(state->manager, &iter)) != -1) {
        struct ECS_Position *pos = ECS_GetComponent(state->manager, eid, ECS_CPosition);
        struct ECS_Movement *move = ECS_GetComponent(state->manager, eid, ECS_CMovement);

        u8 val = state->map[(int)(pos->pos.x + 0.5f)][(int)(pos->pos.y + 0.5f)];
        state->map[(int)(pos->pos.x + 0.5f)][(int)(pos->pos.y + 0.5f)] = 0;

        if (ECS_HasComponent(state->manager, eid, ECS_CBounding)) {
            struct ECS_Bounding *box = ECS_GetComponent(state->manager, eid, ECS_CBounding);
            HandleCollides(state->manager, eid, pos, move, box);
        }

        state->map[(int)(pos->pos.x + 0.5f)][(int)(pos->pos.y + 0.5f)] = val;
        move->vel = V2_Mul(0.9f, move->vel);

        if (0 > pos->pos.x)
            pos->pos.x += 18.f;
        if (pos->pos.x > 18.f)
            pos->pos.x -= 18.f;
        if (0 > pos->pos.y)
            pos->pos.y += 20.f;
        if (pos->pos.y > 20.f)
            pos->pos.y -= 20.f;
    }

    /* Game Logic System */
    struct ECS_Player *player = ECS_GetComponent(state->manager, state->player_eid, ECS_CPlayer);
    iter = (struct ECS_Iter){ ECS_CCollided, 0 };
    while ((eid = ECS_NextEntity(state->manager, &iter)) != -1) {
        struct ECS_Collided *collide = ECS_GetComponent(state->manager, eid, ECS_CCollided);
        if (ECS_HasComponent(state->manager, collide->with, ECS_CEdible)) {
            struct ECS_Edible *edible = ECS_GetComponent(state->manager, collide->with, ECS_CEdible);
            player->score += edible->score;
            if (edible->delete)
                ECS_RemoveEntity(state->manager, collide->with);
        }
        ECS_RemoveComponent(state->manager, eid, ECS_CCollided);
    }
}

/**
 * Render the actual scene onto the screen
 * @memory   : struct of the actual memory
 * @renderer : sdl renderer to display to
 * @dt       : the time with which to adjust when rendering mid-update frame
 *
 * We have to adjust positions slightly based on velocity to get the right
 * render position for each object when doing something mid-frame
 */
extern
RENDER(Render) /* memory, renderer, dt */
{
    /* has been initialized in update */
    struct GameState *state = (struct GameState *)memory->perm_mem;

    int screenw, screenh;
    SDL_RenderGetLogicalSize(renderer, &screenw, &screenh);

    if (!state->init) return;

    struct LocalStack render_stack;
    Z_BeginLocalStack(&render_stack, state->temp_stack);

    SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
    SDL_RenderClear(renderer);

    /* TODO(david): insert render code here */
    struct ECS_Iter iter = { ECS_CRender | ECS_CPosition, 0 };
    i64 eid;
    while ((eid = ECS_NextEntity(state->manager, &iter)) != -1) {
        struct ECS_Position *pos = ECS_GetComponent(state->manager, eid, ECS_CPosition);
        struct ECS_Render *render = ECS_GetComponent(state->manager, eid, ECS_CRender);

        i32 top  = pos->pos.x * PIXEL_PERSQUARE - render->w * PIXEL_PERSQUARE + PIXEL_PERSQUARE;
        i32 left = pos->pos.y * PIXEL_PERSQUARE - render->h * PIXEL_PERSQUARE + PIXEL_PERSQUARE;
        SDL_Rect rect = { top, left,
                          2 * render->w * PIXEL_PERSQUARE, 2 * render->h * PIXEL_PERSQUARE };
        SDL_SetRenderDrawColor(renderer, render->color.r, render->color.g, render->color.b, render->color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    if (I_IsEnabled(state->console) && state->font != NULL) {
        I_RenderConsole(renderer, state->font, state->console);
    }

    SDL_RenderPresent(renderer);

    Z_EndLocalStack(&render_stack);
}

