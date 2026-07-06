#include "world_factory.h"

Room make_room(int w, int h)
{
    Room room = {0};

    room.w = w;
    room.h = h;
    room.doors_count = 0;

    for (int y = 0; y < room.h; y++) {
        for (int x = 0; x < room.w; x++) {

            if (x == 0 || y == 0 ||
                x == room.w - 1 ||
                y == room.h - 1) {
                set_tile(&room, x, y, WALL);
            } else {
                set_tile(&room, x, y, EMPTY);
            }
        }
    }

    return room;
}

Door make_door(int x, int y, int target_node, int spawn_x, int spawn_y)
{
    return (Door){
        .x = x,
        .y = y,
        .target_node = target_node,
        .spawn_x = spawn_x,
        .spawn_y = spawn_y
    };
}

void set_tile(Room* room, int x, int y, TileType tile)
{
	room->tiles[y][x] = tile;
}

void add_marker(Room* room, Marker marker)
{
    if (room->markers_count >= MAX_MARKERS)
        return;

    room->markers[room->markers_count++] = marker;
}

