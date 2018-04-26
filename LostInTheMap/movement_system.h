#pragma once
#include "Space.h"
#include "map_system.h"

#include <math.h>


class movement_system
{
private:
	static float tps;
	static void(*movement_finished)(Entity* mover);
public:
	static void set_movement_finished_callback(void(*movement_finished)(Entity*)) { movement_system::movement_finished = movement_finished; }
	static void move_characters_tick(Space& game_space, int dt, ITerrain* tr);
	static void move_characters_tick_combat(Space& game_space, int dt, ITerrain* tr);
	static void move_character_transform(int dt, SDL_Point cur_dest, Transform * tc, IMoving * mc);
	movement_system();
	~movement_system();
};

