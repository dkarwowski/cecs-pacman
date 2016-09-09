#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#include "main.h"
#include "math.h"

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

    if (I_IsEnabled(state->console) && state->font != NULL) {
        I_RenderConsole(renderer, state->font, state->console);
    }

    SDL_RenderPresent(renderer);

    Z_EndLocalStack(&render_stack);
}

