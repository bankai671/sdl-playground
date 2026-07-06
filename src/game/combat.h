#include "entity_factory.h"

bool is_in_range(Entity* a, Entity* b, float range);
void apply_damage(Entity* attacker, Entity* target, int damage);
bool is_entity_alive(Entity* e);
Entity* find_nearest_target(Entity* source, Entity* entities, int count, EntityType type, float radius);
bool check_attack_hit(Entity* attacker, Entity* victim);

