#pragma once

#include <SDL.h>

extern SDL_Texture* font_texture;
void init_font(SDL_Renderer* r);
void free_font();

void draw_text(SDL_Renderer* r, const char* text, int x, int y, int scale, SDL_Color color);

