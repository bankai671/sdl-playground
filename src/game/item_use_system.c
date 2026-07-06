#include "game.h"
#include "item_use_system.h"

#include "core/logger.h"

bool use_key(Game* game) {
	(void)game;
    LOG_INFO("USE: There is nothing to unlock with this key here.");
    return false;
}

bool use_health(Game* game) {
    PlayerData* p = (PlayerData*)game->world.entities[game->world.player_index].entity_data;
    
    if (p->combat_data.hp >= p->combat_data.max_hp) {
        LOG_INFO("USE: Your health is already full!");
        return false;
    }
    
    p->combat_data.hp += 30;
    if (p->combat_data.hp > p->combat_data.max_hp) {
		p->combat_data.hp = p->combat_data.max_hp;
	}
    
    LOG_INFO("USE: Used First-Aid Kit. +30 HP! Current Health: %d/%d",
			p->combat_data.hp,
			p->combat_data.max_hp
	);

    return true;
}

bool use_sword(Game* game) {
	(void)game;
    LOG_INFO("USE: You swung the sword, but there are no enemies around. (Requires menu to equip)");
    return false;
}

