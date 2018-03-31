#pragma once
#include <vector>

#include "Entity.h"

class character_system
{
private:
	static std::vector<Entity*> characters;
public:
	static std::vector<Entity*> init_characters(Character** character_map, int width, int height, ITerrain* tr);
	static void attach_textures_to_characters(SDL_Point tile_origin);
	character_system();
	~character_system();
};

