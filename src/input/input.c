#include <SDL.h>

#include "input.h"

void inputs(Input* input)
{
	const Uint8* key = SDL_GetKeyboardState(NULL);
	input->w = key[SDL_SCANCODE_W];
	input->a = key[SDL_SCANCODE_A];
	input->s = key[SDL_SCANCODE_S];
	input->d = key[SDL_SCANCODE_D];
}
