#pragma once

#include "core/config.h"

typedef enum {
    ITEM_NONE,
    ITEM_KEY,
    ITEM_HEALTH,
    ITEM_SWORD,
	ITEM_NOTE,
	ITEM_PENDANT,

    ITEM_COUNT
} ItemID;

typedef enum {
    INV_FOCUS_TABS,
    INV_FOCUS_ITEMS,
    INV_FOCUS_ACTION_MENU,
	INV_FOCUS_EXAMINE,
} InvFocus;

typedef enum {
    ITEM_TYPE_ITEMS,
    ITEM_TYPE_SUPPLIES,
    ITEM_TYPE_WEAPONS
} ItemType;

typedef enum {
    ACTION_USE,
    ACTION_EQUIP,
    ACTION_ASSIGN,
    ACTION_EXAMINE,
    ACTION_COUNT
} ItemAction;

typedef struct {
    ItemID items[ITEM_COUNT];
    int count;
	ItemID quick_slots[MAX_QUICK_SLOTS];
	int active_slot;
    ItemID equipped_weapon;
} Inventory;

void inventory_add(Inventory* inv, ItemID id);

