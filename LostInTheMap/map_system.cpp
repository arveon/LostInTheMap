#include "map_system.h"





void map_system::init_terrain_map(int ** tile_ids, levels level, Entity* tilemap)
{
	ITerrain* tc = static_cast<ITerrain*>(tilemap->get_component(ComponentType::Terrain));

	int w = tc->width;
	int h = tc->height;

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			Entity* tile = new Entity(entity_type::game_object, std::to_string(x) + "_" + std::to_string(y));
			IDrawable* dc = new IDrawable(tile, IDrawable::layers::background);
			Transform* tc = new Transform(tile);


		}
	}

}

map_system::map_system()
{
}

map_system::~map_system()
{
}
