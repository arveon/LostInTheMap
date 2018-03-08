#pragma once
#include <string>

#include "Entity.h"

enum levels
{
	test,
	pyramid,
	juji_village,
	caves,
	desert,
	zakra_village
};


class map_system
{
private:
	static levels cur_level;
public:
	static void init_terrain_map(int** tile_ids, levels level, Entity* tilemap);
	map_system();
	~map_system();
};

