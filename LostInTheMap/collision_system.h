#pragma once
#include "Entity.h"
#include "map_system.h"
#include "geometry_utilities.h"

class collision_system
{
public:
	static SDL_Point resolve_collisions(ICollidable* character_collision, IMoving* mc, ITerrain* tc);
	collision_system();
	~collision_system();
};

