#include <stdio.h>

#include <SDL.h>

#include "combat.h"

bool is_in_range(Entity* a, Entity* b, float range) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
 	return (dx * dx + dy * dy) <= (range * range);
}

void apply_damage(Entity* attacker, Entity* target, int damage) {
    if (!target || target->state != STATE_ACTIVE) return;

    int new_hp = 0;
    const char* attacker_name = (attacker->type == ENTITY_PLAYER) ? "Player" : "Enemy";
    const char* target_name = (target->type == ENTITY_PLAYER) ? "Player" : "Enemy";

    if (target->type == ENTITY_PLAYER) {
        PlayerData* p = (PlayerData*)target->entity_data;
        p->combat_data.hp -= damage;
        if (p->combat_data.hp <= 0) { 
            p->combat_data.hp = 0; 
            p->combat_data.is_dead = true; 
        }
        new_hp = p->combat_data.hp;
    } 
    else if (target->type == ENTITY_ENEMY) {
        EnemyData* e = (EnemyData*)target->entity_data;
        e->combat_data.hp -= damage;
        if (e->combat_data.hp <= 0) { 
            e->combat_data.hp = 0; 
            e->combat_data.is_dead = true; 
            target->state = STATE_INACTIVE; 
        }
        new_hp = e->combat_data.hp;
    }

    printf("[COMBAT] %s dealt %d damage to %s. Target HP: %d\n", 
           attacker_name, damage, target_name, new_hp);
    
    if (new_hp == 0) {
        printf("[COMBAT] %s has been defeated.\n", target_name);
    }
}

bool is_entity_alive(Entity* e) {
    if (e->state != STATE_ACTIVE) return false;
    
    if (e->type == ENTITY_PLAYER) {
        return !((PlayerData*)e->entity_data)->combat_data.is_dead;
    }
    if (e->type == ENTITY_ENEMY) {
        return !((EnemyData*)e->entity_data)->combat_data.is_dead;
    }
    return true;
}

Entity* find_nearest_target(Entity* source, Entity* entities, int count, EntityType type, float radius) {
    Entity* nearest = NULL;
    float min_dist = radius * radius;

    for (int i = 0; i < count; i++) {
        Entity* e = &entities[i];
        if (e == source || e->type != type || e->state != STATE_ACTIVE) continue;

        float dx = source->x - e->x;
        float dy = source->y - e->y;
        float dist_sq = dx * dx + dy * dy;
		
		if (dist_sq < min_dist) {
            min_dist = dist_sq;
            nearest = e;
        }
    }
    return nearest;
}

bool check_attack_hit(Entity* attacker, Entity* victim) {
    EnemyData* ed = (EnemyData*)attacker->entity_data;
    PlayerData* pd = (PlayerData*)victim->entity_data;

    float hit_x = attacker->x + ((ed->combat_data.attack_dir_x > 0) ? 32 : -ed->combat_data.attack_hitbox.w);
    float hit_y = attacker->y + ed->combat_data.attack_hitbox.y;

    SDL_Rect attack_rect = { (int)hit_x, (int)hit_y, (int)ed->combat_data.attack_hitbox.w, (int)ed->combat_data.attack_hitbox.h };
    SDL_Rect hurt_rect = { (int)(victim->x + pd->combat_data.hurtbox.x), 
                           (int)(victim->y + pd->combat_data.hurtbox.y), 
                           (int)pd->combat_data.hurtbox.w, 
                           (int)pd->combat_data.hurtbox.h };

    return SDL_HasIntersection(&attack_rect, &hurt_rect);
}

