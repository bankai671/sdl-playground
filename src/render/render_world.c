#include "render.h"
#include "colors.h"

typedef struct {
	int x, y, w, h;
} Quad;


void render_collider(SDL_Renderer* renderer, Game* game, Entity* e);
void render_hurtbox(SDL_Renderer* renderer, Game* game, Entity* e);
void render_attack_hitbox(SDL_Renderer* renderer, Game* game, Entity* e);

static void render_quad(SDL_Renderer* r, Quad* q, Camera* cam, Uint8 cr, Uint8 cg, Uint8 cb)
{
    float sx = q->x - cam->x;
    float sy = q->y - cam->y;

    SDL_Rect rect = {
        (int)(sx + 0.5f),
        (int)(sy + 0.5f),
        q->w,
        q->h
    };

    SDL_SetRenderDrawColor(r, cr, cg, cb, 255);
    SDL_RenderFillRect(r, &rect);
}

static void render_player(SDL_Renderer* r, Quad* q, Camera* cam)
{
	render_quad(r, q, cam, UNPACK_RGB(COLOR_PLAYER));
}

static void render_enemy(SDL_Renderer* r, Quad* q, Camera* cam)
{
	render_quad(r, q, cam, UNPACK_RGB(COLOR_ENEMY));
}

static void render_item(SDL_Renderer* r, Quad* q, Camera* cam)
{
	render_quad(r, q, cam, UNPACK_RGB(COLOR_ITEM_GENERIC));
}

static void render_wall(SDL_Renderer* r, Quad* q, Camera* cam)
{
	render_quad(r, q, cam, UNPACK_RGB(COLOR_WALL));
}

static void render_door(SDL_Renderer* r, Quad* q, Camera* cam)
{
	render_quad(r, q, cam, UNPACK_RGB(COLOR_DOOR));
}

static void render_chest(SDL_Renderer* r, Quad* q, Camera* cam, Entity* e)
{
    if (e->type == ENTITY_CHEST && !((ChestData*)e->entity_data)->is_opened) {
        render_quad(r, q, cam, UNPACK_RGB(COLOR_CHEST_CLOSED));
        return;
    }
    render_quad(r, q, cam, UNPACK_RGB(COLOR_CHEST_OPENED));
}

void render_floor_grid(Game* game, SDL_Renderer* r)
{
	int cam_x = (int)game->camera.x;
    int cam_y = (int)game->camera.y;

    int step = TILE_SIZE / 4;

    int first_grid_x = (cam_x / step) * step;
    if (cam_x < 0) first_grid_x -= step;

    int first_grid_y = (cam_y / step) * step;
    if (cam_y < 0) first_grid_y -= step;

    SDL_Rect bg_rect = { 0, 0, GAME_WIDTH, GAME_HEIGHT };
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_FLOOR_DARK));
    SDL_RenderFillRect(r, &bg_rect);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    for (int x = first_grid_x; x <= cam_x + GAME_WIDTH + step; x += step) {
        int screen_x = x - cam_x;

        int rem = x % TILE_SIZE;
        if (rem < 0) rem += TILE_SIZE;

        if (rem == 0) {
            SDL_SetRenderDrawColor(r, 55, 55, 65, 220); 
            SDL_RenderDrawLine(r, screen_x, 0, screen_x, GAME_HEIGHT);
        } 
        else if (rem == TILE_SIZE / 2) {
            SDL_SetRenderDrawColor(r, 45, 45, 55, 130); 
            SDL_RenderDrawLine(r, screen_x, 0, screen_x, GAME_HEIGHT);
        } 
        else if (rem == TILE_SIZE / 4 || rem == (3 * TILE_SIZE) / 4) {
            SDL_SetRenderDrawColor(r, 40, 40, 45, 50); 
            SDL_RenderDrawLine(r, screen_x, 0, screen_x, GAME_HEIGHT);
        }
    }

    for (int y = first_grid_y; y <= cam_y + GAME_HEIGHT + step; y += step) {
        int screen_y = y - cam_y;

        int rem = y % TILE_SIZE;
        if (rem < 0) rem += TILE_SIZE;

        if (rem == 0) {
            SDL_SetRenderDrawColor(r, 55, 55, 65, 220);
            SDL_RenderDrawLine(r, 0, screen_y, GAME_WIDTH, screen_y);
        } 
        else if (rem == TILE_SIZE / 2) {
            SDL_SetRenderDrawColor(r, 45, 45, 55, 130);
            SDL_RenderDrawLine(r, 0, screen_y, GAME_WIDTH, screen_y);
        } 
        else if (rem == TILE_SIZE / 4 || rem == (3 * TILE_SIZE) / 4) {
            SDL_SetRenderDrawColor(r, 40, 40, 45, 50);
            SDL_RenderDrawLine(r, 0, screen_y, GAME_WIDTH, screen_y);
        }
    }
    
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
}

void render_world_subsystem(Game *game, SDL_Renderer *renderer)
{
	render_floor_grid(game, renderer);

	World* world = &game->world;
    WorldGraph* g = &world->graph;
    Room* room = &g->nodes[g->current_node].room;

    for (int i = 0; i < world->entity_count; i++)
    {
        Entity* e = &world->entities[i];
        if (e->state != STATE_ACTIVE) continue;

        Quad q = {
            e->x,
            e->y,
            e->w,
            e->h
        };

        switch (e->type)
        {
            case ENTITY_PLAYER:
                render_player(renderer, &q, &game->camera);
                break;

            case ENTITY_ENEMY:
                render_enemy(renderer, &q, &game->camera);
                break;

            case ENTITY_ITEM:
                render_item(renderer, &q, &game->camera);
                break;

            case ENTITY_CHEST:
                render_chest(renderer, &q, &game->camera, e);
                break;
        }
    }

	if (game->show_colliders) {
		for (int i = 0; i < game->world.entity_count; ++i) {
			Entity* e = &game->world.entities[i];
			
			if (e->state != STATE_ACTIVE) continue;

			if (e->physics_box.is_solid) {
				render_collider(renderer, game, e);
			}

			if (e->type == ENTITY_PLAYER || e->type == ENTITY_ENEMY) {
            	render_hurtbox(renderer, game, e);
				render_attack_hitbox(renderer, game, e);
        	}
		}
	}

    for (int y = 0; y < room->h; y++) {
        for (int x = 0; x < room->w; x++) {
            if (room->tiles[y][x] == WALL) {
                render_wall(renderer,
                    &(Quad){
                    x * TILE_SIZE,
                    y * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                    },
                    &game->camera
                );
            }
        }
    }

    for (int i = 0; i < room->doors_count; i++) {
        Door* d = &room->doors[i];

        render_door(renderer,
                &(Quad){
                d->x * TILE_SIZE,
                d->y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
                },
                &game->camera
        );
    }
}


void render_collider(SDL_Renderer* renderer, Game* game, Entity* e)
{
    SDL_SetRenderDrawColor(renderer, UNPACK_RGBA(COLOR_COLLIDER));

    SDL_Rect rect = {
        (int)(e->x + e->physics_box.x - game->camera.x), 
        (int)(e->y + e->physics_box.y - game->camera.y),
        (int)(e->physics_box.w),
        (int)(e->physics_box.h)
    };

    SDL_RenderDrawRect(renderer, &rect);
}

void render_hurtbox(SDL_Renderer* renderer, Game* game, Entity* e)
{
    Collider* hb = NULL;

    if (e->type == ENTITY_PLAYER) {
        hb = &((PlayerData*)e->entity_data)->combat_data.hurtbox;
    } else if (e->type == ENTITY_ENEMY) {
        hb = &((EnemyData*)e->entity_data)->combat_data.hurtbox;
    }

    if (hb) {
        SDL_SetRenderDrawColor(renderer, UNPACK_RGBA(COLOR_HURTBOX));

        SDL_Rect rect = {
            (int)(e->x + hb->x - game->camera.x), 
            (int)(e->y + hb->y - game->camera.y),
            (int)hb->w,
            (int)hb->h
        };
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void render_attack_hitbox(SDL_Renderer* renderer, Game* game, Entity* e)
{
    CombatData* cd = (e->type == ENTITY_ENEMY) ? &((EnemyData*)e->entity_data)->combat_data : NULL;

    if (cd && cd->is_attacking) {
        float offset_x = (cd->attack_dir_x > 0) ? 32.0f : -cd->attack_hitbox.w;
        
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 200);
        SDL_Rect hb = {
            (int)(e->x + offset_x - game->camera.x),
            (int)(e->y + cd->attack_hitbox.y - game->camera.y),
            (int)cd->attack_hitbox.w,
            (int)cd->attack_hitbox.h
        };
        SDL_RenderDrawRect(renderer, &hb);
    }
}

