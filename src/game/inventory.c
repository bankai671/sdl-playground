#include "inventory.h"
#include "item_db.h"

void inventory_add(Inventory* inv, ItemID id)
{
    if (id <= ITEM_NONE || id >= ITEM_COUNT)
		return;

	if ((int)inv->items[id] == 0 && ITEM_DATABASE[id].type == ITEM_TYPE_SUPPLIES) {
        for (int i = 0; i < MAX_QUICK_SLOTS; i++) {
            if (inv->quick_slots[i] == ITEM_NONE) {
                inv->quick_slots[i] = id;
                break;
            }
        }
    }

    inv->items[id]++;
	inv->count++;
}

