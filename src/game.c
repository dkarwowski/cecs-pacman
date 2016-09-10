#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#include "main.h"
#include "math.h"
#include "e_factory.h"

#include "game.h"

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

        u8 array[21][19] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
                             { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                             { 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1 },
                             { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                             { 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1 },
                             { 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
                             { 1, 1, 1, 1, 0, 1, 1, 1, 9, 1, 9, 1, 1, 1, 0, 1, 1, 1, 1 },
                             { 9, 9, 9, 1, 0, 1, 9, 9, 9, 2, 9, 9, 9, 1, 0, 1, 9, 9, 9 },
                             { 1, 1, 1, 1, 0, 1, 9, 1, 1, 1, 1, 1, 9, 1, 0, 1, 1, 1, 1 },
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
        for (int r = 0; r < 21; r++) {
            for (int c = 0; c < 19; c++) {
                u8 val = array[r][c];
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
                        ECS_NewPlayer(state->manager, c, r);
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

        if (fabsf(move->vel.x) < 0.0001f)
            move->diff.x = 0.5f;
        if (fabsf(move->vel.y) < 0.0001f)
            move->diff.y = 0.5f;
    }

    /* movement system */
    iter = (struct ECS_Iter){ ECS_CPosition | ECS_CMovement, 0 };
    while ((eid = ECS_NextEntity(state->manager, &iter)) != -1) {
        struct ECS_Position *pos = ECS_GetComponent(state->manager, eid, ECS_CPosition);
        struct ECS_Movement *move = ECS_GetComponent(state->manager, eid, ECS_CMovement);

        move->diff = V2_Add(move->diff, V2_Mul(SEC_PER_UPDATE, move->vel));
        
        int x = move->diff.x + pos->x;
        int y = move->diff.y + pos->y;

        bool collide = false;

        struct ECS_Iter inner = { ECS_CPosition, 0 };
        i64 inner_eid;
        while ((x != pos->x || y != pos->y)
                && (inner_eid = ECS_NextEntity(state->manager, &inner)) != -1) {
            if (inner_eid == eid)
                continue;

            struct ECS_Position *inner_pos = ECS_GetComponent(state->manager, inner_eid, ECS_CPosition);

            if (inner_pos->x == x && inner_pos->y == y) {
                collide = true;
                break;
            }
        }

        if ((x != pos->x || y != pos->y) && !collide) {
            pos->x = x;
            move->diff.x = 0.5f;
            pos->y = y;
            move->diff.y = 0.5f;
        }

        move->vel = (struct Vec2){ 0.f, 0.f };
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

        SDL_Rect rect = { pos->x * PIXEL_PERSQUARE, pos->y * PIXEL_PERSQUARE,
                          render->w * PIXEL_PERSQUARE, render->h * PIXEL_PERSQUARE };
        SDL_SetRenderDrawColor(renderer, render->color.r, render->color.g, render->color.b, render->color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    if (I_IsEnabled(state->console) && state->font != NULL) {
        I_RenderConsole(renderer, state->font, state->console);
    }

    SDL_RenderPresent(renderer);

    Z_EndLocalStack(&render_stack);
}

