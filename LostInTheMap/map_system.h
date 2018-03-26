#pragma once
#include <string>

#include "Entity.h"

typedef enum 
{
	test,
	pyramid,
	juji_village,
	caves,
	desert,
	zakra_village
}levels;


class map_system
{
private:
	static levels cur_level;
	static int tile_width;
public:
	static void init_terrain_map(int** tile_ids, levels level, Entity* tilemap);
	static void init_terrain_collisions(int** collision_map, Entity* tilemap);
	static int** get_pathfinding_map(ITerrain* tilemap);

	static SDL_Point world_to_tilemap_ids(SDL_Point world_coords, ITerrain* tilemap);

	static int get_tile_width() { return tile_width; }
	map_system();
	~map_system();
};

