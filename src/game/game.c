#include "game.h"
#include "ai.h"
#include "item_db.h"
#include "inventory.h"

#include "core/math.h"
#include "core/logger.h"

static void init_ui_context(Game* game);

static void spawn_from_markers(Game* game, Room* room);
static void spawn_chest(Game* game, int x, int y, ItemID item_id, bool is_opened, Marker* marker_ptr);
static void spawn_enemy(Game* game, int x, int y);
static void spawn_item(Game* game, int x, int y, ItemID item_id, int count, Marker* marker_ptr);

static bool collides_tiles(Game* game, float x, float y, float w, float h);
static bool collides_tiles_for_entity(Game* game, Entity* e);
static bool collides_entities(Game* game, Entity* self);

static void enter_room(Game* game, int node);
static bool intersects(Entity* a, Entity* b);
static bool is_on_door(Game* game, Door* d);
static bool is_near(Entity* a, Entity* b);
static bool is_chest_opened(Entity* e);
static void update_auto_pickup(Game* game);
static void update_entities(Game* game, float dt);
static void update_camera(Game* game, float dt);
static void move_entity(Game* game, Entity* e, float dx, float dy);

void handle_interaction(Game* game);

void init_game(Game* game)
{
	game->show_colliders = true;
 	World* world = &game->world;
    world->entity_count = 0;

    WorldGraph* g = &world->graph;
    g->node_count = 2;
    g->current_node = 0;

    g->nodes[0].id = 0;
    g->nodes[0].room = make_room(40, 40);

    g->nodes[1].id = 1;
    g->nodes[1].room = make_room(20, 20);

    Room* r0 = &g->nodes[0].room;
    Room* r1 = &g->nodes[1].room;

    add_marker(r0, (Marker){
			.x = 10,
			.y = 10,
			.type = MARKER_ENEMY_SPAWN,
			.data.enemy = (EnemyMarkerData){
				.enemy_type = 0
			}
	});

    add_marker(r0, (Marker){
			.x = 8,
			.y = 8,
			.type = MARKER_ITEM,
			.data.item = (ItemMarkerData){
				.id = ITEM_KEY,
				.count = 1,
			}
	});

    add_marker(r0,  (Marker){
			.x = 18,
			.y = 8,
			.type = MARKER_ITEM,
			.data.item = (ItemMarkerData){
				.id = ITEM_HEALTH,
				.count = 1,
			},
	});

    add_marker(r0,  (Marker){
			.x = 9,
			.y = 16,
			.type = MARKER_ITEM,
			.data.item = (ItemMarkerData){
				.id = ITEM_HEALTH,
				.count = 1,
			},
	});

    int dx0 = r0->w / 2;
	int dy0 = 0;

	set_tile(r0, dx0, dy0, EMPTY);

	r0->doors[0] = make_door(
		dx0, dy0, 
		1, 
		r1->w / 2, r1->h - 3 
	);
	r0->doors_count = 1;

	int dx1 = r1->w / 2;
	int dy1 = r1->h - 1;

	set_tile(r1, dx1, dy1, EMPTY);

	r1->doors[0] = make_door(
		dx1, dy1, 
		0, 
		r0->w / 2, 2
	);
	r1->doors_count = 1;

	add_marker(r1, (Marker){
			.x = 10,
			.y = 10,
			.type = MARKER_CHEST,
			.data.chest = (ChestMarkerData){
				.loot = ITEM_SWORD
			},
	});

	add_marker(r1, (Marker){
			.x = 15,
			.y = 15,
			.type = MARKER_ITEM,
			.data.item = (ItemMarkerData){
				.id = ITEM_PENDANT,
				.count = 1,
			}
	});

    world->player_index = world->entity_count++;
    world->entities[world->player_index] = make_player();

	Entity* player = &world->entities[world->player_index];
	player->x = 5 * TILE_SIZE;
    player->y = 5 * TILE_SIZE;

    enter_room(game, 0);
	
	init_ui_context(game);

    game->camera.x = player->x - (float)GAME_WIDTH / 2;
    game->camera.y = player->y - (float)GAME_HEIGHT / 2;
}

static void update_movement(Game* game, float dt)
{
    World* world = &game->world;
    Entity* player = &world->entities[world->player_index];

	float dx = 0, dy = 0;

    if (game->input.a) dx -= PLAYER_SPEED * dt;
    if (game->input.d) dx += PLAYER_SPEED * dt;
    if (game->input.w) dy -= PLAYER_SPEED * dt;
    if (game->input.s) dy += PLAYER_SPEED * dt;

    move_entity(game, player, dx, dy);
}

static void update_rooms(Game* game)
{
    World* world = &game->world;
    WorldGraph* g = &world->graph;

    Room* room = &g->nodes[g->current_node].room;

    for (int i = 0; i < room->doors_count; i++) {
        Door* d = &room->doors[i];

        if (is_on_door(game, d)) {
			LOG_INFO("Door triggered (%d,%d)", d->x, d->y);

    		LOG_INFO("Transition %d -> %d",
             g->current_node,
             d->target_node);
			
            enter_room(game, d->target_node);
			
			Entity* player = &game->world.entities[game->world.player_index];

            player->x = d->spawn_x * TILE_SIZE;
            player->y = d->spawn_y * TILE_SIZE;

			game->camera.x = player->x - (float)GAME_WIDTH / 2;
			game->camera.y = player->y - (float)GAME_HEIGHT / 2;

			LOG_INFO("Player spawned at (%.0f, %.0f)",
             player->x, player->y);

            return;
        }
    }
}

static void update_camera(Game* game, float dt)
{
    World* world = &game->world;
	WorldGraph* g = &world->graph;

	Room* room = &g->nodes[g->current_node].room;
    Entity* player = &world->entities[world->player_index];

    float room_pixel_w = room->w * TILE_SIZE;
    float room_pixel_h = room->h * TILE_SIZE;

	float max_x = room_pixel_w - GAME_WIDTH;
	float max_y = room_pixel_h - GAME_HEIGHT;

	float target_x = clampf(player->x - (float)GAME_WIDTH / 2, 0, max_x);
    float target_y = clampf(player->y - (float)GAME_HEIGHT / 2, 0, max_y);


    if (max_x < 0) max_x = 0;
    if (max_y < 0) max_y = 0;
	
	float lerp_speed = 12.0f;

	game->camera.x += (target_x - game->camera.x) * lerp_speed * dt;
    game->camera.y += (target_y - game->camera.y) * lerp_speed * dt;
}

void handle_interaction(Game* game)
{
	LOG_INFO("Interact pressed");

    World* world = &game->world;

    Entity* player = &world->entities[world->player_index];

    for (int i = 0; i < world->entity_count; i++) {

        if (i == world->player_index)
            continue;

        Entity* e = &world->entities[i];

  		if (e->state != STATE_ACTIVE)
			continue;

        if (!is_near(player, e))
            continue;
		
		if (e->type != ENTITY_CHEST)
			continue;
		
		if (e->type == ENTITY_CHEST && is_chest_opened(e))
			continue;

        if (e->state == STATE_ACTIVE && e->type == ENTITY_CHEST) {
			PlayerData* player_data = (PlayerData*)player->entity_data;
			ChestData* chest_data = (ChestData*)e->entity_data;

			LOG_INFO("Player inventory items count after chest open: %d", player_data->inventory.count);

			chest_data->is_opened = true;

			inventory_add(&player_data->inventory, chest_data->item_id);

			if (chest_data && chest_data->marker_ptr) {
				Marker* m = chest_data->marker_ptr;
				m->data.chest.is_opened = true;
			}

            LOG_INFO("Chest opened");
			LOG_INFO("Chest item id: %d", chest_data->item_id);
			LOG_INFO("Chest is_opened: %d", chest_data->is_opened);
			
			LOG_INFO("Player inventory items count after chest open: %d", player_data->inventory.count);

            continue;
        }
    }
}

static void update_auto_pickup(Game* game)
{
    World* world = &game->world;
    Entity* player = &world->entities[world->player_index];

    for (int i = 0; i < world->entity_count; i++) {
        if (i == world->player_index) continue;

        Entity* e = &world->entities[i];

        if (e->state != STATE_ACTIVE) continue;
        if (e->type != ENTITY_ITEM) continue;

        if (intersects(player, e)) {
			e->state = STATE_INACTIVE;

			ItemData* data = (ItemData*)e->entity_data;
			inventory_add(&((PlayerData*)player->entity_data)->inventory, data->id);
			if (data && data->marker_ptr) {
				Marker* m = data->marker_ptr;
				m->consumed = true;
			}

            LOG_INFO("Auto picked item");
        }
    }
}

void update_game(Game* game, float dt)
{
	update_movement(game, dt);
	update_auto_pickup(game);
	update_rooms(game);
	update_entities(game, dt);
	update_camera(game, dt);
}

static bool collides_tiles(Game* game, float x, float y, float w, float h)
{
	World* world = &game->world;
    WorldGraph* g = &world->graph;
    Room* room = &g->nodes[g->current_node].room;

    int left   = (int)(x / TILE_SIZE);
    int right  = (int)((x + w - 1) / TILE_SIZE);
    int top    = (int)(y / TILE_SIZE);
    int bottom = (int)((y + h - 1) / TILE_SIZE);

    if (left < 0 || right >= room->w || top < 0 || bottom >= room->h)
        return true;

    return (
        room->tiles[top][left] == WALL ||
        room->tiles[top][right] == WALL ||
        room->tiles[bottom][left] == WALL ||
        room->tiles[bottom][right] == WALL
    );
}

static bool collides_tiles_for_entity(Game* game, Entity* e)
{
    return collides_tiles(game, e->x + e->physics_box.x, e->y + e->physics_box.y, 
                          e->physics_box.w, e->physics_box.h);
}

static bool collides_entities(Game* game, Entity* self)
{
	World* world = &game->world;

    for (int i = 0; i < world->entity_count; i++) {
        Entity* e = &world->entities[i];

        if (e == self || e->state != STATE_ACTIVE || !e->physics_box.is_solid) {
            continue;
        }

        float self_left   = self->x + self->physics_box.x;
        float self_right  = self->x + self->physics_box.x + self->physics_box.w;
        float self_top    = self->y + self->physics_box.y;
        float self_bottom = self->y + self->physics_box.y + self->physics_box.h;

        float other_left   = e->x + e->physics_box.x;
        float other_right  = e->x + e->physics_box.x + e->physics_box.w;
        float other_top    = e->y + e->physics_box.y;
        float other_bottom = e->y + e->physics_box.y + e->physics_box.h;

        if (self_right > other_left &&
            self_left < other_right &&
            self_bottom > other_top &&
            self_top < other_bottom) 
        {
            return true; 
        }
    }
    return false;
}

static bool is_on_door(Game* game, Door* d)
{
    Entity* p = &game->world.entities[game->world.player_index];

    float dx = d->x * TILE_SIZE;
    float dy = d->y * TILE_SIZE;

    return (
        p->x <= dx + TILE_SIZE &&
        p->x + p->w >= dx &&
        p->y <= dy + TILE_SIZE &&
        p->y + p->h >= dy
    );
}

static void spawn_from_markers(Game* game, Room* room)
{
	for (int i = 0; i < room->markers_count; i++) {
        Marker* m = &room->markers[i];
		
		if (m->consumed)
			continue;

        switch (m->type) {
             case MARKER_ENEMY_SPAWN:
                spawn_enemy(game, m->x, m->y);
                break;

            case MARKER_ITEM:
                spawn_item(game, m->x, m->y, m->data.item.id, m->data.item.count, m);
                break;

			case MARKER_CHEST:
				spawn_chest(game, m->x, m->y, m->data.chest.loot, m->data.chest.is_opened, m);
				break;

            default:
                break;
        }
	}
}

static void spawn_enemy(Game* game, int x, int y)
{
    World* world = &game->world;

    Entity* e = &world->entities[world->entity_count++];
    *e = make_enemy();

	e->x = x * TILE_SIZE;
	e->y = y * TILE_SIZE;

    LOG_INFO("Enemy spawned at %d,%d", x, y);
}

static void
spawn_item(
		Game* game,
		int x,
		int y,
		ItemID item_id,
		int count,
		Marker* marker_ptr
		)
{
    World* world = &game->world;

    Entity* e = &world->entities[world->entity_count++];
    *e = make_item(item_id, count);

	e->x = x * TILE_SIZE;
	e->y = y * TILE_SIZE;

    LOG_INFO("Item spawned at %d,%d", x, y);
	((ItemData*)e->entity_data)->marker_ptr = marker_ptr;
}

static void
spawn_chest(
		Game* game,
		int x,
		int y,
		ItemID item_id,
		bool is_opened,
		Marker* marker_ptr
		)
{
	World* world = &game->world;

    Entity* e = &world->entities[world->entity_count++];
    *e = make_chest(item_id);

	e->x = x * TILE_SIZE;
	e->y = y * TILE_SIZE;

    LOG_INFO("Chest spawned at %d,%d", x, y);

	ChestData* chest_data = (ChestData*)e->entity_data;
	chest_data->marker_ptr = marker_ptr;
	chest_data->is_opened = is_opened;
}

static void enter_room(Game* game, int node)
{
   LOG_INFO("Entering room: %d", node);

    World* world = &game->world;
    WorldGraph* g = &world->graph;

    g->current_node = node;
    Room* room = &g->nodes[node].room;
	
	for (int i = world->player_index + 1; i < world->entity_count; i++) {
    	destroy_entity(&world->entities[i]); 
	}

    world->entity_count = world->player_index + 1;

    spawn_from_markers(game, room);
}

static bool is_near(Entity* a, Entity* b)
{
    float dx = a->x - b->x;
    float dy = a->y - b->y;

    return dx * dx + dy * dy < 64 * 64;
}

static bool intersects(Entity* a, Entity* b)
{
    return !(a->x + a->w < b->x ||
             a->x > b->x + b->w ||
             a->y + a->h < b->y ||
             a->y > b->y + b->h);
}

static bool is_chest_opened(Entity* e)
{
	return ((ChestData*)e->entity_data)->is_opened;
}

static void init_ui_context(Game* game)
{
    game->ui.focus = INV_FOCUS_ITEMS;
	game->ui.active_tab = 0;
    game->ui.item_cursor = 0;
	game->ui.action_cursor = 0;
}

void update_entities(Game* game, float dt) {
    Entity* player = &game->world.entities[game->world.player_index];

    for (int i = 0; i < game->world.entity_count; i++) {
        Entity* e = &game->world.entities[i];
        if (e->type == ENTITY_ENEMY && e->state == STATE_ACTIVE) {
            update_enemy_ai(e, player, dt);
			move_entity(game, e, e->vx * dt, e->vy * dt);
        }
    }
}

static void move_entity(Game* game, Entity* e, float dx, float dy)
{
    e->x += dx;
    if (collides_tiles_for_entity(game, e) || collides_entities(game, e)) {
        e->x -= dx;
    }

    e->y += dy;
    if (collides_tiles_for_entity(game, e) || collides_entities(game, e)) {
        e->y -= dy;
    }
}

