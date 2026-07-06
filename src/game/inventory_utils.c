#include "item_db.h"

int get_tab_item_count(Inventory* inv, int tab)
{
    int count = 0;
    for (int id = 1; id < ITEM_COUNT; id++) {
        if (inv->items[id] > 0 && ITEM_DATABASE[id].type == (ItemType)tab) {
            count++;
        }
    }
    return count;
}

ItemID get_item_by_tab_cursor(Inventory* inv, int tab, int cursor)
{
    int current_idx = 0;
    for (int id = 1; id < ITEM_COUNT; id++) {
        if (inv->items[id] > 0 && ITEM_DATABASE[id].type == (ItemType)tab) {
            if (current_idx == cursor) {
                return (ItemID)id;
            }
            current_idx++;
        }
    }
    return ITEM_NONE;
}

int get_available_actions(ItemType type, ItemAction* actions)
{
    int count = 0;
    switch (type) {
        case ITEM_TYPE_SUPPLIES:
            actions[count++] = ACTION_USE;
            actions[count++] = ACTION_ASSIGN;
            actions[count++] = ACTION_EXAMINE;
            break;
        case ITEM_TYPE_WEAPONS:
            actions[count++] = ACTION_EQUIP;
            actions[count++] = ACTION_EXAMINE;
            break;
        case ITEM_TYPE_ITEMS:
            actions[count++] = ACTION_USE;
            actions[count++] = ACTION_EXAMINE;
            break;
    }
    return count;
}

const char* get_action_name(ItemAction action)
{
    switch (action) {
        case ACTION_USE:     return "USE";
        case ACTION_EQUIP:   return "EQUIP";
        case ACTION_ASSIGN:  return "ASSIGN";
        case ACTION_EXAMINE: return "EXAMINE";
        default:             return "UNKNOWN";
    }
}

