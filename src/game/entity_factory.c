#include <stdlib.h>

#include "entity_factory.h"

Entity make_player()
{
	PlayerData* player_data = calloc(1, sizeof(PlayerData));

	Collider hurtbox = {
		.x = -2,
		.y = -2,
		.w = TILE_SIZE + 4,
		.h = TILE_SIZE + 4,
		.is_solid = false,
	};

	Collider attack_hitbox = {
		.x = 0,
		.y = 0,
		.w = 32,
		.h = 32,
		.is_solid = false,
	};

	CombatData combat_data = {
		.max_hp = 100,
		.hp = 10,
		.hurtbox = hurtbox,
		.attack_hitbox = attack_hitbox,
	};

	*player_data = (PlayerData){
		.combat_data = combat_data,
		.inventory = {0},
	};
		
	Collider c = {
		.x = 0,
		.y = 0,
		.w = TILE_SIZE,
		.h = TILE_SIZE,
		.is_solid = true,
	};

	return (Entity){
		.type = ENTITY_PLAYER,
		.x = 0,
		.y = 0,
		.vx = 0,
		.vy = 0,
		.w = 32,
		.h = 32,
		.physics_box = c,
		.state = STATE_ACTIVE,
		.entity_data = player_data,
	};
}

Entity make_enemy()
{
	EnemyData* enemy_data = calloc(1, sizeof(EnemyData));
	
	Collider hurtbox = {
		.x = -6,
		.y = -6,
		.w = TILE_SIZE + 12,
		.h = TILE_SIZE + 12,
		.is_solid = false,
	};
	
	Collider attack_hitbox = {
		.x = 0,
		.y = 0,
		.w = 32,
		.h = 32,
		.is_solid = false,
	};

	CombatData combat_data = {
		.max_hp = 100,
		.hp = 100,
		.is_dead = false,
		.is_attacking = false,
		.hurtbox = hurtbox,
		.attack_hitbox = attack_hitbox,
	};

	*enemy_data = (EnemyData){
		.combat_data = combat_data,
		.marker_ptr = NULL,
	};
	
	Collider c = {
		.x = 2,
		.y = 2,
		.w = TILE_SIZE - 4,
		.h = TILE_SIZE - 4,
		.is_solid = true,
	};

	return (Entity){
        .type = ENTITY_ENEMY,
        .x = 0,
        .y = 0,
        .w = 32,
        .h = 32,
        .vx = 0,
        .vy = 0,
		.physics_box = c,
		.state = STATE_ACTIVE,
		.entity_data = enemy_data,
	};
}

Entity make_item(ItemID item_id, int count)
{
	ItemData* item_data = calloc(1, sizeof(ItemData));

	*item_data = (ItemData){
		.id = item_id,
		.count = count,
		.marker_ptr = NULL,
	};

	Collider c = {
		.x = 0,
		.y = 0,
		.w = 16,
		.h = 16,
		.is_solid = false,
	};

	return (Entity){
        .type = ENTITY_ITEM,
        .x = 0,
        .y = 0,
        .w = 16,
        .h = 16,
		.physics_box = c,
		.state = STATE_ACTIVE,
		.entity_data = item_data,
    };
}

Entity make_chest(ItemID item_id)
{
	ChestData* chest_data = calloc(1, sizeof(ChestData));

	*chest_data = (ChestData){
		.item_id = item_id,
		.marker_ptr = NULL,
	};

	Collider c = {
		.x = 0,
		.y = 0,
		.w = 64,
		.h = 32,
		.is_solid = true,
	};

	return (Entity){
		.type = ENTITY_CHEST,
		.x = 0,
		.y = 0,
		.w = 64,
		.h = 32,
		.physics_box = c,
		.state = STATE_ACTIVE,
		.entity_data = chest_data,
	};
}

void destroy_entity(Entity* e)
{
    if (!e->entity_data) return;
	
	free(e->entity_data);

	e->entity_data = NULL;
    e->state = STATE_INACTIVE;
}

