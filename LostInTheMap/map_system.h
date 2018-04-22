#pragma once
#include <string>

#include "Entity.h"
#include "SpaceSystem.h"
#include "director.h"
#include "Levels.h"
#include "ObjectTypes.h"
#include "NameToTypeConversion.h"


class map_system
{
private:
	static levels cur_level;
	static int tile_width;
public:
	static void init_terrain_map(int** tile_ids, Entity* tilemap);
	static void init_terrain_collisions(int** collision_map, Entity* tilemap);
	static std::vector<Entity*> init_triggers(Actor** trigger_map, ITerrain* tr);
	static std::vector<Entity*> init_objects(Actor ** objects_map, ITerrain * tr);
	static int** get_pathfinding_map(ITerrain* tilemap);

	static Entity* get_tile_at(Entity* terrain, SDL_Point world_coords);

	static SDL_Point world_to_tilemap_ids(SDL_Point world_coords, ITerrain* tilemap);
	static SDL_Point tilemap_ids_to_world(SDL_Point ids, ITerrain* tilemap);

	static int get_tile_width() { return tile_width; }
	map_system();
	~map_system();
};

