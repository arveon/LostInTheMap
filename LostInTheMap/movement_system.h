#pragma once
#include "Space.h"
#include <math.h>


class movement_system
{
private:
	static int millis_per_tile;
	static int elapsed_walking;
	static float tps;

public:
	static void move_characters_tick(Space& game_space, int dt, ITerrain* tr);
	movement_system();
	~movement_system();
};

