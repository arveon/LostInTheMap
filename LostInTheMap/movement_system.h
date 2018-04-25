#pragma once
#include "Space.h"
#include "map_system.h"

#include <math.h>


class movement_system
{
private:
	static float tps;
public:
	static void move_characters_tick(Space& game_space, int dt, ITerrain* tr);
	static void move_character_transform(int dt, SDL_Point cur_dest, Transform * tc, IMoving * mc);
	movement_system();
	~movement_system();
};

