#include <stdio.h>

#include "render.h"
#include "colors.h"

#include "game/inventory.h"
#include "game/item_db.h"
#include "game/inventory_utils.h"
#include "core/config.h"
#include "core/font.h"

static void render_hud(Game* game, SDL_Renderer* r);
static void render_quick_slots(Game* game, SDL_Renderer* r);
static void render_inventory_screen(Game* game, SDL_Renderer* r);
static void render_inventory_base(SDL_Renderer* r);
static void render_inventory_list(Game* game, SDL_Renderer* renderer, int start_x, int start_y, int item_w, int item_h, int padding);
static void render_inventory_preview(Game* game, SDL_Renderer* render, int view_x, int view_y, int view_w, int view_h);
static void render_context_action_menu(Game* game, SDL_Renderer* r, int list_x, int list_y);
static void render_examine_mode(Game* game, SDL_Renderer* r);

void render_ui_subsystem(Game* game, SDL_Renderer* r)
{
    render_hud(game, r);
	render_quick_slots(game, r);

	if (game->state == STATE_INVENTORY) {
		render_inventory_screen(game, r);
	}

    if (game->ui.focus == INV_FOCUS_EXAMINE && 
        game->ui.examine_handler != NULL && 
        game->ui.examine_handler->type == EXAMINE_TYPE_FULLSCREEN) 
    {
        render_examine_mode(game, r);
    }
}

static void render_hud(Game* game, SDL_Renderer* r)
{
    PlayerData* p = (PlayerData*)game->world.entities[game->world.player_index].entity_data;

    float hp = (float)p->combat_data.hp / p->combat_data.max_hp;

    SDL_Rect hp_bg = {20, 20, 200, 10};
    SDL_Rect hp_fg = {20, 20, (int)(200 * hp), 10};

    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_HP_BG));
    SDL_RenderFillRect(r, &hp_bg);
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_HP_FG));
    SDL_RenderFillRect(r, &hp_fg);
}

static void render_quick_slots(Game* game, SDL_Renderer* r)
{
    World* world = &game->world;
    PlayerData* p = (PlayerData*)world->entities[world->player_index].entity_data;
    Inventory* inv = &p->inventory;

    int slot_size = 32;
    int padding = 6;
    int total_width = (slot_size * MAX_QUICK_SLOTS) + (padding * (MAX_QUICK_SLOTS - 1));
    int start_x = 20;
    int y_pos = GAME_HEIGHT - slot_size - padding; 

    SDL_Rect hud_bg = { start_x - padding, y_pos - padding, total_width + (padding * 2), slot_size + (padding * 2) };
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_SLOTS_BG));
    SDL_RenderFillRect(r, &hud_bg);

    for (int i = 0; i < MAX_QUICK_SLOTS; i++) {
        ItemID id = inv->quick_slots[i];
        int count = (id != ITEM_NONE) ? inv->items[id] : 0;
        SDL_Rect slot_rect = { start_x, y_pos, slot_size, slot_size };
        
        if (id != ITEM_NONE && count > 0) {
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_SLOT_FILLED));
        } else {
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_SLOT_EMPTY));
        }
        SDL_RenderFillRect(r, &slot_rect);

        if (id != ITEM_NONE && count > 0) {
            SDL_Rect item_indicator = { start_x + 6, y_pos + 6, slot_size - 12, slot_size - 12 };
            switch (id) {
                case ITEM_HEALTH: SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_ITEM_HEALTH)); break;
                default:          SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_ITEM_GENERIC)); break;
            }
            SDL_RenderFillRect(r, &item_indicator);
        }

        if (i == (int)inv->active_slot) {
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_SLOT_HIGHLIGHT)); 
        } else {
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_SLOT_BORDER));
        }
        SDL_RenderDrawRect(r, &slot_rect);
        start_x += slot_size + padding;
    }
}

static void render_inventory_screen(Game* game, SDL_Renderer* r)
{
	UIContext* ui = &game->ui;

    int list_x = 50;
    int list_y = 110;
    int item_w = 110; 
    int item_h = 32;
    int padding = 8;

    int view_x = 180;
    int view_y = 180;
    int view_w = GAME_WIDTH - view_x - 40;
    int view_h = GAME_HEIGHT - view_y - 40;

    render_inventory_base(r);
    render_inventory_list(game, r, list_x, list_y, item_w, item_h, padding);
	
	if (game->ui.focus != INV_FOCUS_EXAMINE) {
        render_inventory_preview(game, r, view_x, view_y, view_w, view_h);
    }

    if (ui->focus == INV_FOCUS_ACTION_MENU) {
        render_context_action_menu(game, r, list_x, list_y);
    }

	if (ui->focus == INV_FOCUS_EXAMINE && ui->examine_handler->type != EXAMINE_TYPE_FULLSCREEN) {
        render_examine_mode(game, r);
    }
}

void draw_text_wrapped(SDL_Renderer* r, const char* desc, int x, int y, int max_width, SDL_Color color)
{
    char line_buffer[128];
    int buf_idx = 0;
    int max_chars_per_line = max_width / 6;
    int desc_y = y;
    int word_start = 0;
    int i = 0;
    
    while (desc[i] != '\0') {
        while (desc[i] == ' ') i++;
        if (desc[i] == '\0') break;
        
        word_start = i;
        while (desc[i] != ' ' && desc[i] != '\0') i++;
        int word_len = i - word_start;
        
        if (buf_idx + word_len + (buf_idx > 0 ? 1 : 0) > max_chars_per_line) {
            line_buffer[buf_idx] = '\0';
            draw_text(r, line_buffer, x, desc_y, 1, color);
            desc_y += 11;
            buf_idx = 0;
        }
        
        if (buf_idx > 0) {
            line_buffer[buf_idx++] = ' ';
        }
        
        for (int j = 0; j < word_len; j++) {
            if (buf_idx < (int)sizeof(line_buffer) - 1) {
                line_buffer[buf_idx++] = desc[word_start + j];
            }
        }
    }
    
    if (buf_idx > 0) {
        line_buffer[buf_idx] = '\0';
        draw_text(r, line_buffer, x, desc_y, 1, color);
    }
}

static void render_inventory_base(SDL_Renderer* r)
{
    SDL_Rect overlay = { 0, 0, GAME_WIDTH, GAME_HEIGHT };
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_OVERLAY));
    SDL_RenderFillRect(r, &overlay);

    SDL_Rect main_frame = { 30, 30, GAME_WIDTH - 60, GAME_HEIGHT - 60 };
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_FRAME));
    SDL_RenderDrawRect(r, &main_frame);
        
    draw_text(r, "INVENTORY", 50, 45, 2, COLOR_INV_TEXT_HEADER);
    
    SDL_Rect header_line = { 50, 65, 120, 4 };
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_HEADER_LINE));
    SDL_RenderFillRect(r, &header_line);
    
    SDL_Rect header_subline = { 50, 73, 70, 2 };
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_HEADER_SUBLINE));
    SDL_RenderFillRect(r, &header_subline);
}

static void render_inventory_list(Game* game, SDL_Renderer* r, int start_x, int start_y, int item_w, int item_h, int padding)
{
	World* world = &game->world;
	UIContext* ui = &game->ui;
    PlayerData* p = (PlayerData*)world->entities[world->player_index].entity_data;
    Inventory* inv = &p->inventory;

    const char* tab_names[] = { "ITEMS", "SUPPLIES", "WEAPONS" };
    
    for (int i = 0; i < 3; i++) {
        int current_tab_y = start_y + (i * (item_h + padding));
        SDL_Rect tab_rect = { start_x, current_tab_y, item_w, item_h };
        
        if (i == ui->active_tab) {
            SDL_Color t_color = (ui->focus == INV_FOCUS_TABS) ? COLOR_INV_TEXT_GOLD : COLOR_INV_TEXT_WHITE;
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_BAR_ACTIVE_BG));
            SDL_RenderFillRect(r, &tab_rect);
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_BAR_ACTIVE_BORDER));
            SDL_RenderDrawRect(r, &tab_rect);
            draw_text(r, tab_names[i], start_x + 10, current_tab_y + 12, 1, t_color);
        } else {
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_BAR_INACTIVE_BG));
            SDL_RenderFillRect(r, &tab_rect);
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_BAR_INACTIVE_BORD));
            SDL_RenderDrawRect(r, &tab_rect);
            draw_text(r, tab_names[i], start_x + 10, current_tab_y + 12, 1, COLOR_INV_HEADER_SUBLINE);
        }
    }

    int ribbon_x = start_x + item_w + 20;
    int ribbon_y = start_y;
    int box_size = 50;
    int box_padding = 10;
    
    int items_in_tab = get_tab_item_count(inv, ui->active_tab);
    
    if (items_in_tab == 0) {
        draw_text(r, "[ EMPTY ]", ribbon_x, ribbon_y + 15, 1, COLOR_INV_HEADER_SUBLINE);
        return;
    }

    for (int i = 0; i < items_in_tab; i++) {
        ItemID id = get_item_by_tab_cursor(inv, ui->active_tab, i);
        int count = inv->items[id];
        
        int current_box_x = ribbon_x + (i * (box_size + box_padding));
        SDL_Rect box_rect = { current_box_x, ribbon_y, box_size, box_size };
        
        if (ui->focus != INV_FOCUS_TABS && i == ui->item_cursor) {
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_BAR_ACTIVE_BG));
            SDL_RenderFillRect(r, &box_rect);
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_BAR_ACTIVE_BORDER));
        } else {
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_BAR_INACTIVE_BG));
            SDL_RenderFillRect(r, &box_rect);
            SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_BAR_INACTIVE_BORD));
        }
        SDL_RenderDrawRect(r, &box_rect);
        
        SDL_Rect icon_rect = { current_box_x + 12, ribbon_y + 12, box_size - 24, box_size - 24 };
        switch (id) {
            case ITEM_HEALTH: SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_ITEM_HEALTH)); break;
            case ITEM_SWORD:  SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_ITEM_SWORD_MINI)); break;
            case ITEM_KEY:    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_ITEM_KEY_MINI)); break;
			case ITEM_PENDANT: SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_ITEM_PENDANT_CLOSED)); break;
            default:          SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_ITEM_DEFAULT_MINI)); break;
        }

        SDL_RenderFillRect(r, &icon_rect);
        
        if (ui->active_tab == 1) {
            char cnt_str[16];
            snprintf(cnt_str, sizeof(cnt_str), "%d", count);
            draw_text(r, cnt_str, current_box_x + box_size - 14, ribbon_y + box_size - 12, 1, COLOR_INV_TEXT_GOLD);
        }
    }
}

static void render_inventory_preview(Game* game, SDL_Renderer* r, int view_x, int view_y, int view_w, int view_h)
{
	World* world = &game->world;
	UIContext* ui = &game->ui;
    PlayerData* p = (PlayerData*)world->entities[world->player_index].entity_data;
    Inventory* inv = &p->inventory;

    SDL_Rect view_rect = { view_x, view_y, view_w, view_h };
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_VIEW_BG));
    SDL_RenderFillRect(r, &view_rect);
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_VIEW_BORDER));
    SDL_RenderDrawRect(r, &view_rect);

    ItemID active_id = get_item_by_tab_cursor(inv, ui->active_tab, ui->item_cursor);
    if (active_id == ITEM_NONE || inv->items[active_id] <= 0) return;

    draw_text(r, ITEM_DATABASE[active_id].name, view_x + 20, view_y + 20, 1, COLOR_INV_TEXT_GOLD);

    SDL_Rect line = { view_x + 20, view_y + 36, view_w - 40, 2 };
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_LINE_SEP));
    SDL_RenderFillRect(r, &line);

    draw_text_wrapped(r, ITEM_DATABASE[active_id].description, view_x + 20, view_y + 48, view_w - 40, COLOR_INV_TEXT_WHITE);
}

static void render_context_action_menu(Game* game, SDL_Renderer* r, int list_x, int list_y)
{
	World* world = &game->world;
	UIContext* ui = &game->ui;
    PlayerData* p = (PlayerData*)world->entities[world->player_index].entity_data;
    Inventory* inv = &p->inventory;

 	ItemID current_item = get_item_by_tab_cursor(inv, ui->active_tab, ui->item_cursor);
    if (current_item == ITEM_NONE) return;

    ItemAction actions[ACTION_COUNT];
    int action_count = get_available_actions(ITEM_DATABASE[current_item].type, actions);

    int menu_w = 120;
    int row_h = 24;
    int menu_h = action_count * row_h;
    
    int menu_x = list_x + 130 + (ui->item_cursor * 60);
    int menu_y = list_y + 55;

    SDL_Rect menu_bg = { menu_x, menu_y, menu_w, menu_h };
    SDL_SetRenderDrawColor(r, 15, 15, 20, 250);
    SDL_RenderFillRect(r, &menu_bg);
    SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_INV_FRAME));
    SDL_RenderDrawRect(r, &menu_bg);

    for (int i = 0; i < action_count; i++) {
        const char* action_name = get_action_name(actions[i]);
        
        int text_x = menu_x + 22; 
        int text_y = menu_y + (i * row_h) + (row_h - 10) / 2; 

        SDL_Color text_color = COLOR_INV_TEXT_WHITE;

        if (i == ui->action_cursor) {
            text_color = COLOR_INV_TEXT_GOLD;
            draw_text(r, ">", menu_x + 8, text_y, 1, COLOR_INV_TEXT_GOLD);
        }
        
        draw_text(r, action_name, text_x, text_y, 1, text_color);
    }
}

static void render_examine_mode(Game* game, SDL_Renderer* r)
{
	UIContext* ui = &game->ui;
    if (ui->focus != INV_FOCUS_EXAMINE || ui->examine_handler == NULL) return;

    if (ui->examine_handler->type == EXAMINE_TYPE_FULLSCREEN) {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, UNPACK_RGBA(COLOR_OVERLAY_DARK));
        SDL_Rect screen_rect = { 0, 0, GAME_WIDTH, GAME_HEIGHT };
        SDL_RenderFillRect(r, &screen_rect);
        ui->examine_handler->draw(game, r, ui->examine_data);
    } else {
        SDL_Rect bg = { 180, 180, GAME_WIDTH - 220, GAME_HEIGHT - 220 };
        SDL_SetRenderDrawColor(r, 20, 20, 25, 255);
        SDL_RenderFillRect(r, &bg);
        
        ui->examine_handler->draw(game, r, ui->examine_data);
    }
};

