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
	movement_system();
	~movement_system();
};

