#include <stdlib.h>

#include "item_db.h"
#include "examine_system.h"
#include "item_use_system.h"

#include "render/colors.h"

static const ExamineText KEY_EXAMINE_DATA = { 
    .description = "An old iron key. The number '102' is faintly engraved on it." 
};

static const ExamineText HEALTH_EXAMINE_DATA = {
	.description = "A standard first-aid kit. It can heal wounds."
};

static const ExamineText SWORD_EXAMINE_DATA = {
	.description = "A heavy steel sword. The blade is razor-sharp." 
};

static const ExaminePages NOTE_EXAMINE_DATA = {
	.pages = (const char*[]){
		"Page 1: Text 1",
		"Page 2: Text 2",
		"Page 3: Text 3"
	},
	.page_count = 5,
	.current_page = 0,
};

static const ExamineColored PENDANT_EXAMINE_DATA = {
	.description = "A silver pendant. It glows with a faint green light.",
	.highlight_color = COLOR_ITEM_PENDANT_OPENED,
};

const ItemDefinition ITEM_DATABASE[ITEM_COUNT] = {
    [ITEM_NONE]   = { 
		"Empty",
		"Nothing here",
		ITEM_TYPE_ITEMS,
		NULL,
		NULL,
		NULL,
	},
    [ITEM_KEY]    = {
		.name = "Rusty Key",
		.description = "Key to heavy metal door. ",
		.type = ITEM_TYPE_ITEMS,
		.examine_data = (void*)&KEY_EXAMINE_DATA,
		.handler = &EXAMINE_HANDLER_TEXT,
		.use = use_key
	},
    [ITEM_HEALTH] = {
		.name = "First-Aid Kit",
		.description = "Restores HP",
		.type = ITEM_TYPE_SUPPLIES,
		.examine_data = (void*)&HEALTH_EXAMINE_DATA,
		.handler = &EXAMINE_HANDLER_TEXT,
		use_health
	},
    [ITEM_SWORD]  = {
		.name = "Steel Sword",
		.description = "A sharp blade. ",
		.type = ITEM_TYPE_WEAPONS,
		.examine_data = (void*)&SWORD_EXAMINE_DATA,
		.handler = &EXAMINE_HANDLER_TEXT,
		use_sword
	},
	[ITEM_NOTE] = {
		.name = "Mysterious Note",
		.description = "A scrap of paper.",
		.type = ITEM_TYPE_ITEMS,
		.examine_data = (void*)&NOTE_EXAMINE_DATA,
		.handler = &EXAMINE_HANDLER_PAGES,
		NULL
	},
	[ITEM_PENDANT] = {
		.name = "Old Pendant",
		.description = "A tarnished silver pendant...",
		.type = ITEM_TYPE_ITEMS,
		.examine_data = (void*)&PENDANT_EXAMINE_DATA,
		.handler = &EXAMINE_HANDLER_PENDANT,
	}
};

