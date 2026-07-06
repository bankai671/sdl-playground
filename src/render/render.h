#pragma once

#include <SDL.h>

#include "game/game.h"

void render(Game* game, SDL_Renderer* renderer);

void render_world_subsystem(Game* game, SDL_Renderer* renderer);
void render_ui_subsystem(Game* game, SDL_Renderer* renderer);

void draw_text_wrapped(SDL_Renderer* r, const char* desc, int x, int y, int max_width, SDL_Color color);
