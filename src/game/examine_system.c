#include "game.h"
#include "examine_system.h"

#include "core/font.h"
#include "render/render.h"
#include "render/colors.h"

static void examine_draw_text(Game* g, SDL_Renderer* r, void* data) {
	(void)g;
    ExamineText* d = (ExamineText*)data;
    draw_text_wrapped(r, d->description, 200, 200, 300, COLOR_INV_TEXT_WHITE);
}

static void examine_draw_colored(Game* g, SDL_Renderer* r, void* data) {
	(void)g;
    ExamineColored* d = (ExamineColored*)data;
    draw_text_wrapped(r, d->description, 200, 200, 300, COLOR_INV_TEXT_WHITE);
    draw_text(r, "(!)", 200, 300, 1, d->highlight_color);
}

static void examine_draw_pages(Game* g, SDL_Renderer* r, void* data) {
	(void)g;
    ExaminePages* d = (ExaminePages*)data;
    draw_text_wrapped(r, d->pages[d->current_page], 200, 200, 300, COLOR_INV_TEXT_WHITE);
}

void examine_draw_pendant(Game* game, SDL_Renderer* r, void* data) 
{
	(void)game;
    ExamineColored* d = (ExamineColored*)data;
    
    SDL_Rect icon = { GAME_WIDTH / 2 - 50, 100, 100, 100 };
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(d->highlight_color));
    SDL_RenderFillRect(r, &icon);
    
    draw_text_wrapped(r, d->description, 200, 250, 300, COLOR_INV_TEXT_WHITE);
}

static void input_pages(Game* g, void* data, int dir) {
	(void)g;
    ExaminePages* d = (ExaminePages*)data;
    d->current_page = (d->current_page + dir + d->page_count) % d->page_count;
}

const ExamineHandler EXAMINE_HANDLER_TEXT    = { .type = EXAMINE_TYPE_SIMPLE, 	  .draw = examine_draw_text,    .input = NULL };
const ExamineHandler EXAMINE_HANDLER_COLORED = { .type = EXAMINE_TYPE_FULLSCREEN, .draw = examine_draw_colored, .input = NULL };
const ExamineHandler EXAMINE_HANDLER_PAGES   = { .type = EXAMINE_TYPE_FULLSCREEN, .draw = examine_draw_pages,   .input = input_pages };
const ExamineHandler EXAMINE_HANDLER_PENDANT = { .type = EXAMINE_TYPE_FULLSCREEN, .draw = examine_draw_pendant, .input = NULL };

