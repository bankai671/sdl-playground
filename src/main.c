#include "core/config.h"

#include "game/game.h"
#include "game/item_db.h"
#include "game/inventory_utils.h"
#include "game/combat.h"
#include "engine/engine.h"
#include "render/render.h"
#include "core/font.h"

void loop(Game* game, Engine* engine, SDL_Renderer* renderer);
void events(bool* event_loop_running, Game* game);

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_StopTextInput();

	SDL_Window* window = SDL_CreateWindow(
			"SDL Window test",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
	if (!window) {
    	printf("Window error: %s\n", SDL_GetError());
    	return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer) {
    	printf("Renderer error: %s\n", SDL_GetError());
    	SDL_DestroyWindow(window);
    	return 1;
	}

	SDL_RenderSetLogicalSize(renderer, GAME_WIDTH, GAME_HEIGHT);
	
	init_font(renderer);

	Engine engine = {0};
	init_engine(&engine);

	Game game = {0};
	init_game(&game);

	loop(&game, &engine, renderer);
	
	free_font();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void loop(Game* game, Engine* engine, SDL_Renderer* renderer)
{
	bool event_loop_running = true;

	while (event_loop_running) {
		events(&event_loop_running, game);

		engine->dt = calc_dt(engine);
		update_fps(engine);
		
		if (game->state == STATE_GAMEPLAY) {
            inputs(&game->input); 
            update_game(game, engine->dt);
        } else {
            game->input.w = game->input.a = game->input.s = game->input.d = false;
        }

		render(game, renderer);
	}
}

void events(bool* event_loop_running, Game* game)
{
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			*event_loop_running = false;
		}

		if (event.type == SDL_KEYDOWN && !event.key.repeat) {
			if (event.key.keysym.scancode == SDL_SCANCODE_I) {
                if (game->state == STATE_GAMEPLAY) {
                    game->state = STATE_INVENTORY;
					
					UIContext* ui = &game->ui;

					ui->focus = INV_FOCUS_TABS;
					ui->active_tab = 0;
					ui->item_cursor = 0;
					ui->action_cursor = 0;
                } else {
                    game->state = STATE_GAMEPLAY;
                }
                continue;
            }

			if (game->state == STATE_INVENTORY) {
				PlayerData* p = (PlayerData*)game->world.entities[game->world.player_index].entity_data;
				Inventory* inv = &p->inventory;
				UIContext* ui = &game->ui;

				int items_in_tab = get_tab_item_count(inv, ui->active_tab);

				if (ui->focus == INV_FOCUS_EXAMINE) {
					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE || 
							event.key.keysym.scancode == SDL_SCANCODE_RETURN || 
							event.key.keysym.scancode == SDL_SCANCODE_E || 
							event.key.keysym.scancode == SDL_SCANCODE_Q) {
						ui->focus = INV_FOCUS_ITEMS;
					}
					continue;
				}


				if (ui->focus == INV_FOCUS_TABS) {
					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						game->state = STATE_GAMEPLAY;
						continue;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_W || event.key.keysym.scancode == SDL_SCANCODE_UP) {
						ui->active_tab = (ui->active_tab - 1 + 3) % 3;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_S || event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
						ui->active_tab = (ui->active_tab + 1) % 3;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_E) {
						if (items_in_tab > 0) {
							ui->focus = INV_FOCUS_ITEMS;
							ui->item_cursor = 0;
						}
					}
					continue;
				}

				if (ui->focus == INV_FOCUS_ITEMS) {
					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE || event.key.keysym.scancode == SDL_SCANCODE_Q) {
						ui->focus = INV_FOCUS_TABS;
						continue;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_A || event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
						ui->item_cursor = (ui->item_cursor - 1 + items_in_tab) % items_in_tab;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_D || event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
						ui->item_cursor = (ui->item_cursor + 1) % items_in_tab;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_E) {
						ui->focus = INV_FOCUS_ACTION_MENU;
						ui->action_cursor = 0;
					}
					continue;
				}

				if (ui->focus == INV_FOCUS_ACTION_MENU) {
					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE || event.key.keysym.scancode == SDL_SCANCODE_Q) {
						ui->focus = INV_FOCUS_ITEMS;
						continue;
					}
					
					ItemID current_item = get_item_by_tab_cursor(inv, ui->active_tab, ui->item_cursor);
					ItemAction actions[ACTION_COUNT];
					int action_count = get_available_actions(ITEM_DATABASE[current_item].type, actions);

					if (event.key.keysym.scancode == SDL_SCANCODE_W || event.key.keysym.scancode == SDL_SCANCODE_UP) {
						ui->action_cursor = (ui->action_cursor - 1 + action_count) % action_count;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_S || event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
						ui->action_cursor = (ui->action_cursor + 1) % action_count;
					}
					
					if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_E) {
						ItemAction chosen = actions[ui->action_cursor];
						
						if (chosen == ACTION_USE) {
							if (ITEM_DATABASE[current_item].use != NULL) {
								bool was_consumed = ITEM_DATABASE[current_item].use(game);
								if (was_consumed) {
									inv->items[current_item]--;
									inv->count--;
								}
							}
							if (get_tab_item_count(inv, ui->active_tab) == 0) {
								ui->focus = INV_FOCUS_TABS;
							} else {
								ui->focus = INV_FOCUS_ITEMS;
								ui->item_cursor = 0;
							}
						} 
						else if (chosen == ACTION_EQUIP) {
							inv->equipped_weapon = current_item;
							ui->focus = INV_FOCUS_TABS;
						} 
						else if (chosen == ACTION_ASSIGN) {
							inv->quick_slots[0] = current_item;
							ui->focus = INV_FOCUS_ITEMS;
						} 
						else if (chosen == ACTION_EXAMINE) {
							const ItemDefinition* def = &ITEM_DATABASE[current_item];
							if (def->examine_data != NULL && def->handler != NULL) {
								game->ui.examine_data = (void*)def->examine_data;
								game->ui.examine_handler = def->handler;
								game->ui.focus = INV_FOCUS_EXAMINE;
							}
						}
					}
					continue;
				}
			}

			if (game->state == STATE_GAMEPLAY) {
				if (event.key.keysym.scancode == SDL_SCANCODE_E) {
					handle_interaction(game);
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_F) {
					Entity* player = &game->world.entities[game->world.player_index];
					Entity* enemy = find_nearest_target(player, 
														game->world.entities, 
														game->world.entity_count, 
														ENTITY_ENEMY, 
														50.0f);

					if (enemy) {
						apply_damage(player, enemy, 10);
					} else {
						printf("[COMBAT] Swung at air.\n");
					}
				}

				PlayerData* p = (PlayerData*)game->world.entities[game->world.player_index].entity_data;
				Inventory* inv = &p->inventory;

				if (event.key.keysym.scancode == SDL_SCANCODE_Q) {
					inv->active_slot = (inv->active_slot + 1) % MAX_QUICK_SLOTS;
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_R) {
					ItemID current_item = inv->quick_slots[inv->active_slot];

					if (current_item != ITEM_NONE && inv->items[current_item] > 0) {
						if (ITEM_DATABASE[current_item].use != NULL) {
							bool was_consumed = ITEM_DATABASE[current_item].use(game);
							
							if (was_consumed) {
								inv->items[current_item]--;
								inv->count--;
							}
						}
					}
				}
			}
		}
	}
}

