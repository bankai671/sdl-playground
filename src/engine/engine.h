#pragma once

#include <SDL.h>

typedef struct {
	float dt;
	Uint32 last_time;

	int frames;
	Uint32 fps_timer;
	int fps;
} Engine;

void init_engine(Engine* e);
float calc_dt(Engine* e);
void update_fps(Engine* e);

