#include <math.h>

#include "ai.h"
#include "combat.h"

void update_enemy_ai(Entity* enemy, Entity* player, float dt)
{
    EnemyData* entity_data = (EnemyData*)enemy->entity_data;
	CombatData* combat_data = &entity_data->combat_data;

    if (combat_data->attack_cooldown > 0) {
        combat_data->attack_cooldown -= dt;
    }
    
    if (combat_data->is_attacking) {
        combat_data->attack_hitbox_timer -= dt;
        
        if (combat_data->attack_hitbox_timer <= 0) {
            combat_data->is_attacking = false;
        } else {
            if (enemy->vx >= 0) { 
                combat_data->attack_hitbox.x = 32;
            } else {
                combat_data->attack_hitbox.x = -32;
            }
            combat_data->attack_hitbox.y = 0;
        }
    }
 
    if (combat_data->is_winding_up) {
		combat_data->attack_dir_x = (player->x >= enemy->x) ? 1.0f : -1.0f;
        combat_data->windup_timer -= dt;

        enemy->vx *= 0.5f; 
        enemy->vy *= 0.5f;

        if (combat_data->windup_timer <= 0) {
           combat_data->is_winding_up = false;
            combat_data->is_attacking = true;
            combat_data->attack_hitbox_timer = 0.2f;
            combat_data->attack_cooldown = 3.5f;

            if (check_attack_hit(enemy, player)) {
                apply_damage(enemy, player, 10);
            }
        }
        return;
    }

    float dx = player->x - enemy->x;
    float dy = player->y - enemy->y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist <= 40.0f) {
        enemy->vx = 0;
        enemy->vy = 0;
        if (combat_data->attack_cooldown <= 0) {
            combat_data->is_winding_up = true;
            combat_data->windup_timer = 0.5f;
        }
    } else if (dist < 300.0f) {
        float speed = 100.0f;
        enemy->vx = (dx / dist) * speed;
        enemy->vy = (dy / dist) * speed;    
    } else {
        enemy->vx = 0;
        enemy->vy = 0;
    }
}

