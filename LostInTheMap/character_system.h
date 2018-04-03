#pragma once
#include <vector>

#include "Entity.h"
#include "collision_system.h"
#include "map_system.h"

class character_system
{
private:
	static std::vector<Entity*> characters;
public:
	static std::vector<Entity*> init_characters(Character** character_map, int width, int height, ITerrain* tr);
	static void attach_textures_to_characters(SDL_Point tile_origin);

	//sets destination of character in pathfinding, destination in raw world coords (will be adjusted to character origin automatically)
	static void set_destination(ITerrain* terrain, Entity* character, SDL_Point dest);
	character_system();
	~character_system();
};

