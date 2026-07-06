#pragma once

#include <stdbool.h>

#include "inventory.h"

typedef enum {
    STATE_ACTIVE,
    STATE_INACTIVE,
} EntityState;

typedef enum {
	ENTITY_PLAYER,
	ENTITY_ENEMY,
	ENTITY_ITEM,
	ENTITY_CHEST,
} EntityType;

typedef struct {
	float x, y;
	float w, h;
	bool is_solid;
} Collider;

typedef struct {
	ItemID id;
	int count;
	void* marker_ptr;
} ItemData;

typedef struct {
	int hp;
    int max_hp;
	bool is_dead;
	bool is_winding_up;
	bool is_attacking;
	float windup_timer;
	float attack_cooldown;
	float attack_range;
	float attack_hitbox_timer;
	float attack_dir_x;
	Collider hurtbox;
	Collider attack_hitbox;
} CombatData;

typedef struct {
   	CombatData combat_data;
	Inventory inventory;
} PlayerData;

typedef struct {
    CombatData combat_data;
	void* marker_ptr;
} EnemyData;

typedef struct {
	ItemID item_id;
	bool is_opened;
	void* marker_ptr;
} ChestData;

typedef struct {
	float x, y;
	float vx, vy;
	int w, h;
	EntityType type;
	EntityState state;
	Collider physics_box;
	void* entity_data;
} Entity;

Entity make_player();
Entity make_enemy();
Entity make_item(ItemID item_id, int count);
Entity make_chest(ItemID item_id);
void destroy_entity(Entity* e);

