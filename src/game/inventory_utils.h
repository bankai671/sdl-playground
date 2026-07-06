#pragma once

#include "inventory.h"

int get_tab_item_count(Inventory* inv, int tab);
ItemID get_item_by_tab_cursor(Inventory* inv, int tab, int cursor);
int get_available_actions(ItemType type, ItemAction* actions);
const char* get_action_name(ItemAction action);
