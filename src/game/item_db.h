#pragma once

#include <stdbool.h>

#include <SDL.h>

#include "examine_system.h"
#include "inventory.h"

struct Game; 
typedef struct Game Game;

typedef struct {
    const char* name;
    const char* description;
	ItemType type;

	void* examine_data;
	const ExamineHandler* handler;
    bool (*use)(Game* game);
} ItemDefinition;

extern const ItemDefinition ITEM_DATABASE[ITEM_COUNT];

