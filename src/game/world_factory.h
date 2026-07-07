#include <stdint.h>
#include <stdbool.h>

#include "inventory.h"

#include "core/config.h"

typedef enum {
	EMPTY,
	WALL,
} TileType;

typedef enum {
	MARKER_ENEMY_SPAWN,
	MARKER_ITEM,
	MARKER_CHEST,
} MarkerType;

typedef struct {
    ItemID id;
    int count;
} ItemMarkerData;

typedef struct {
    ItemID loot;
	bool is_opened;
} ChestMarkerData;

typedef struct {
    int enemy_type;
} EnemyMarkerData;

typedef struct {
    int x, y;
    MarkerType type;
	bool consumed;

    union {
        ItemMarkerData item;
        ChestMarkerData chest;
        EnemyMarkerData enemy;
    } data;
} Marker;

typedef struct {
	int x, y;
	int target_node;
	int spawn_x;
	int spawn_y;
} Door;

typedef struct {
	int w, h;
	Door doors[MAX_DOORS];
	int doors_count;

	uint8_t tiles[ROOM_MAX_HEIGHT][ROOM_MAX_WIDTH];

	Marker markers[MAX_MARKERS];
	int markers_count;
} Room;

Room make_room(int w, int h);
Door make_door(int x, int y, int target_node, int spawn_x, int spawn_y);

void set_tile(Room* room, int x, int y, TileType tile);
void add_marker(Room* room, Marker marker);

