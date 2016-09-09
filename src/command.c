#include "command.h"
#include "memory.h"
#include "game.h"

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

#include "main.h"

struct Console {
    bool enabled;
    char buffer[10][128];
};

/**
 * Initialize a console properly
 *
 * @console : the console with buffer
 * @input   : the game input
 */
struct Console *
I_NewConsole(struct Stack *stack, struct GameInput *input)
{
    struct Console *result = Z_PushStruct(stack, struct Console, true);
    result->enabled = false;
    /* TODO(david): remove hard coded buffer length */
    for (int i = 0; i < 10; i++) {
        result->buffer[i][0] = '\0';
    }

    SDL_StopTextInput();
    input->input_text[2] = '\0';
    input->input_len = 2;

    return result;
}

/* Compare macro to make it more legible */
#define I_COMPARE(input_text, command) (strcmp(input_text + 2, command) == 0)

/**
 * Execute a console command that was entered
 *
 * @state : the current game state in case it needs to be manipulated
 * @input : the current state of the input keys
 *
 * The current command should still be stored in the input->input_text
 * field. It'll work either way, just come out as empty space. Any
 * command that isn't handled is replaced with invalid.
 */
static
void
I_ExecuteCommand(struct GameState *state, struct GameInput *input)
{
    if (I_COMPARE(input->input_text, "reload")) {
        input->reload_lib = true;
    } else if (I_COMPARE(input->input_text, "restart")) {
        input->reload_lib = true;
        state->init = false;
    } else if (I_COMPARE(input->input_text, "quit")) {
        input->quit.was_down = true;
    } else if (I_COMPARE(input->input_text, "clear")) {
        for (int i = 0; i < 10; state->console->buffer[i++][0] = '\0') /* that's it */;
        input->input_text[2] = '\0';
        input->input_len = 2;
    } else if (I_COMPARE(input->input_text, "")) {
        /* do nothing */
    } else { /* text entered was invalid, so say so */
        memcpy(input->input_text + 2, "invalid", 8);
        input->input_len = 10;
    }

    memcpy(state->console->buffer[1], input->input_text + 2, input->input_len - 1);

    /* cleanup the input text now */
    input->input_entered = false;
    input->input_text[2] = '\0';
    input->input_len = 2;
}

/**
 * Grab acommand from input and handle it
 *
 * @state : the game state for manipulation
 * @input : input for the console
 */
void
I_HandleCommand(struct GameState *state, struct GameInput *input)
{
    if (input->input_entered && input->input_len > 0) {
        for (int i = 9; i > 1; i--)
            memcpy(state->console->buffer[i], state->console->buffer[i-1], sizeof(state->console->buffer[i]));
        I_ExecuteCommand(state, input);
    } else {
        input->input_entered = false;
    }

    /* always perform this copy, ensures our final buffer is updated */
    memcpy(state->console->buffer[0], input->input_text, input->input_len + 1);
}

/**
 * Enable/disable the console
 *
 * @console : the console with buffer
 */
void
I_ToggleConsole(struct Console *console)
{
    console->enabled = !console->enabled;
    if (console->enabled)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
}

/**
 * Check if the console is enabled
 *
 * @console : console with buffer
 *
 * @return : whether or not the console is up
 */
bool
I_IsEnabled(struct Console *console)
{
    return console->enabled;
}

/**
 * Render the console onto the renderer
 *
 * @renderer : where to place the console
 * @font     : font to use
 * @console  : console with the buffer
 */
void
I_RenderConsole(SDL_Renderer *renderer, TTF_Font *font, struct Console *console)
{
    int width, height;
    SDL_RenderGetLogicalSize(renderer, &width, &height);
    int line_height = TTF_FontLineSkip(font);
    SDL_Rect rect_console = { 20, height - 20 - line_height * 10 - 10, width - 40, line_height * 10 + 10 };
    SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
    SDL_RenderFillRect(renderer, &rect_console);

    SDL_Color fg_color = { 255, 255, 255, 255 };
    SDL_Color bg_color = {  10,  10,  10, 255 };
    for (int i = 0; i < 10; i++) {
        if (console->buffer[i][0] != '\0') {
            SDL_Surface *surface = TTF_RenderText_Shaded(font, console->buffer[i], fg_color, bg_color);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            int tw, th;
            SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
            SDL_Rect r = { 25, height - 25 - TTF_FontLineSkip(font) * (i+1), tw, th };
            SDL_RenderCopy(renderer, texture, NULL, &r);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}
