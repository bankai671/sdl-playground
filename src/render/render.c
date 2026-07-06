#include <stdio.h>

#include "render.h"
#include "colors.h"

#include "game/item_db.h"

void render(Game* game, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, UNPACK_RGBA(COLOR_BG));
    SDL_RenderClear(renderer);
    
	render_world_subsystem(game, renderer);
   	render_ui_subsystem(game, renderer);

    SDL_RenderPresent(renderer);
}

