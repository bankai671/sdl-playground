#include "engine.h"

#include "core/logger.h"


void init_engine(Engine* e)
{
	e->last_time = SDL_GetTicks64();
	e->dt = 0.0f;
	e->frames = 0;
	e->fps = 0;
	e->fps_timer = SDL_GetTicks64();
}

float calc_dt(Engine* e)
{
	Uint32 now = SDL_GetTicks64();
	float dt = (now - e->last_time) / 1000.0f;
	e->last_time = now;

	return dt;
}

void update_fps(Engine* e)
{
    e->frames++;

    Uint32 now = SDL_GetTicks64();

    if (now - e->fps_timer >= 1000) {
        e->fps = e->frames;
        e->frames = 0;
        e->fps_timer = now;
		LOG_INFO("FPS: %d", e->fps);
    }

}

