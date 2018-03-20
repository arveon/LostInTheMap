#pragma once
#include <vector>

#include "Entity.h"

class character_system
{
private:

public:
	static std::vector<Entity*> init_characters(int** collision_map, int width, int height, ITerrain* tr);
	character_system();
	~character_system();
};

