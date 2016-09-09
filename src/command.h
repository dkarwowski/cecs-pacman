#ifndef _COMMAND_h_
#define _COMMAND_h_

#include "config.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct Console;
struct Stack;
struct GameInput;
struct GameState;

struct Console *I_NewConsole(struct Stack *stack, struct GameInput *input);
void            I_HandleCommand(struct GameState *state, struct GameInput *input);
void            I_ToggleConsole(struct Console *console);
bool            I_IsEnabled(struct Console *console);
void            I_RenderConsole(SDL_Renderer *renderer, TTF_Font *font, struct Console *console);

#endif
