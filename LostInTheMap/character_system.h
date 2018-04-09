#pragma once
#include <vector>

#include "Entity.h"
#include "collision_system.h"
#include "map_system.h"
#include "director.h"

#include "xml_system.h"

class character_system
{
private:
	static std::vector<Entity*> characters;
public:
	static std::vector<Entity*> init_characters(Character** character_map, int width, int height, ITerrain* tr);
	static void attach_textures_to_characters(SDL_Point tile_origin);

	

	//sets final destination of character in raw world coords (will be adjusted to character origin automatically)
	static void set_final_destination(ITerrain* terrain, Entity* character, SDL_Point dest, Space& space);
	static SDL_Point get_character_ids(Entity* character, ITerrain* tc);
	character_system();
	~character_system();
};

