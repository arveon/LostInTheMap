#include "map_system.h"





void map_system::init_terrain_map(int ** tile_ids, levels level, Entity* tilemap)
{
	ITerrain* tc = static_cast<ITerrain*>(tilemap->get_component(ComponentType::Terrain));

	int w = tc->width;
	int h = tc->height;
	int t_w = tc->tile_width;

	//initialise all possible tiles in the map to a nullptr
	tc->terrain_tiles = new Entity**[h];
	for (int y = 0; y < h; y++)
	{
		tc->terrain_tiles[y] = new Entity*[w];
		for (int x = 0; x < w; x++)
		{
			tc->terrain_tiles[y][x] = nullptr;
		}
	}

	//loop through all of the tiles, create entities for them (with names representing the coordinate in the grid) and store tile texture id's in draw component id field
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			Entity* tile = new Entity(entity_type::game_object, std::to_string(x) + "_" + std::to_string(y));
			IDrawable* dc = new IDrawable(tile, IDrawable::layers::background);
			tile->add_component(dc);
			dc->id = tile_ids[y][x];
			Transform* transf_c = new Transform(tile);
			tile->add_component(transf_c);
			tc->terrain_tiles[y][x] = tile;

		}
	}

	//display all tile texture id's for testing purposes
	/*for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			IDrawable* dc = static_cast<IDrawable*>(tc->terrain_tiles[y][x]->get_component(ComponentType::Drawable));
			std::cout << dc->id << " ";
		}
		std::cout << std::endl;
	}*/
}

map_system::map_system()
{
}

map_system::~map_system()
{
}
