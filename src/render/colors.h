#pragma once

#include <SDL.h>

#define UNPACK_RGBA(c) (c).r, (c).g, (c).b, (c).a
#define UNPACK_RGB(c)  (c).r, (c).g, (c).b

#define COLOR_BG                    (SDL_Color){20,  20,  20,  255}
#define COLOR_WALL                  (SDL_Color){255, 255, 255, 255}
#define COLOR_DOOR                  (SDL_Color){165, 42,  42,  255}

#define COLOR_PLAYER                (SDL_Color){200, 50,  50,  255}
#define COLOR_ENEMY                 (SDL_Color){0,   0,   255, 255}
#define COLOR_ITEM_GENERIC          (SDL_Color){0,   255, 0,   255}
#define COLOR_CHEST_CLOSED          (SDL_Color){255, 255, 0,   255}
#define COLOR_CHEST_OPENED          (SDL_Color){101, 67,  33,  255}

#define COLOR_FLOOR_DARK            (SDL_Color){24,  24,  28,  255}
#define COLOR_FLOOR_LIGHT           (SDL_Color){34,  34,  38,  255}

#define COLOR_HUD_TEXT              (SDL_Color){255, 255, 255, 255}
#define COLOR_HP_BG                 (SDL_Color){80,  0,   0,   255}
#define COLOR_HP_FG                 (SDL_Color){200, 0,   0,   255}
#define COLOR_MANA_BG               (SDL_Color){0,   0,   80,  255}
#define COLOR_MANA_FG               (SDL_Color){0,   0,   255, 255}

#define COLOR_SLOTS_BG              (SDL_Color){10,  10,  10,  150}
#define COLOR_SLOT_FILLED           (SDL_Color){40,  40,  45,  255}
#define COLOR_SLOT_EMPTY            (SDL_Color){20,  20,  25,  255}
#define COLOR_SLOT_HIGHLIGHT        (SDL_Color){200, 180, 50,  255}
#define COLOR_SLOT_BORDER           (SDL_Color){80,  80,  25,  255}

#define COLOR_ITEM_HEALTH           (SDL_Color){200, 50,  50,  255}
#define COLOR_ITEM_HEALTH_BIG       (SDL_Color){220, 40,  40,  255}
#define COLOR_ITEM_SWORD            (SDL_Color){180, 180, 180, 255}
#define COLOR_ITEM_SWORD_MINI       (SDL_Color){150, 150, 150, 255}
#define COLOR_ITEM_SWORD_BIG        (SDL_Color){170, 170, 175, 255}
#define COLOR_ITEM_KEY              (SDL_Color){200, 200, 0,   255}
#define COLOR_ITEM_KEY_MINI         (SDL_Color){200, 180, 0,   255}
#define COLOR_ITEM_KEY_BIG          (SDL_Color){240, 200, 0,   255}
#define COLOR_ITEM_DEFAULT_MINI     (SDL_Color){40,  180, 40,  255}
#define COLOR_ITEM_DEFAULT_BIG      (SDL_Color){50,  200, 50,  255}
#define COLOR_ITEM_NOTE_SECRET      (SDL_Color){255, 100, 100, 255}

#define COLOR_INV_OVERLAY           (SDL_Color){12,  10,  10,  240}
#define COLOR_INV_FRAME             (SDL_Color){60,  20,  20,  255}
#define COLOR_INV_TEXT_HEADER       (SDL_Color){200, 40,  40,  255}
#define COLOR_INV_TEXT_WHITE        (SDL_Color){255, 255, 255, 255}
#define COLOR_INV_TEXT_GOLD         (SDL_Color){255, 215, 0,   255}
#define COLOR_INV_HEADER_LINE       (SDL_Color){180, 40,  40,  255}
#define COLOR_INV_HEADER_SUBLINE    (SDL_Color){100, 100, 100, 255}
#define COLOR_INV_BAR_ACTIVE_BG     (SDL_Color){50,  15,  15,  255}
#define COLOR_INV_BAR_ACTIVE_BORDER (SDL_Color){200, 30,  30,  255}
#define COLOR_INV_BAR_INACTIVE_BG   (SDL_Color){20,  20,  22,  255}
#define COLOR_INV_BAR_INACTIVE_BORD (SDL_Color){45,  45,  48,  255}
#define COLOR_INV_VIEW_BG           (SDL_Color){18,  16,  16,  255}
#define COLOR_INV_VIEW_BORDER       (SDL_Color){45,  30,  30,  255}
#define COLOR_INV_PREVIEW_BG        (SDL_Color){28,  24,  24,  255}
#define COLOR_INV_PREVIEW_BORDER    (SDL_Color){80,  40,  40,  255}
#define COLOR_INV_LINE_SEP          (SDL_Color){80,  40,  40,  255}
#define COLOR_OVERLAY_DARK          (SDL_Color){0,    0,   0,  220}
#define COLOR_ITEM_PENDANT_CLOSED   (SDL_Color){138, 43, 226,  255}
#define COLOR_ITEM_PENDANT_OPENED   (SDL_Color){124, 252,  0,  255}
#define COLOR_EXAMINE_ACTIVE  		(SDL_Color){144, 238, 144, 255}
#define COLOR_COLLIDER              (SDL_Color){0,   255,   0, 255}
#define COLOR_HURTBOX              	(SDL_Color){0,   0,   255, 255}
#define COLOR_ATTACK_HITBOX         (SDL_Color){255,   0,   0, 200}


