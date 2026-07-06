#pragma once

#include <SDL.h>

#include <stdlib.h>
#include <stdbool.h>

#include "item_db.h"
#include "entity_factory.h"
#include "world_factory.h"

#include "input/input.h"
#include "core/config.h"

typedef struct {
	float x;
	float y;
} Camera;

typedef struct {
	int id;
	Room room;
} WorldNode;

typedef struct {
	WorldNode nodes[MAX_ROOMS];
	int node_count;
	int current_node;
} WorldGraph;

typedef struct {
	WorldGraph graph;
	Entity entities[MAX_ENTITIES];
	int entity_count;
	int player_index;
} World;

typedef enum {
    STATE_GAMEPLAY,
    STATE_INVENTORY,
} GameState;

typedef struct {
	InvFocus focus;
    int active_tab;
    int item_cursor;
    int action_cursor;
    
    void* examine_data;
	const ExamineHandler* examine_handler;
} UIContext;

typedef struct Game {
	Input input;
	World world;
	Camera camera;
	GameState state;
	UIContext ui;
	bool show_colliders;
} Game;

void init_game(Game* game);
void update_game(Game* game, float dt);
void handle_interaction(Game* game);

