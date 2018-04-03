#pragma once
#include "Entity.h"
#include "map_system.h"


class player_system
{
private:
	static Entity* player;
public:
	static void set_player(Entity* player) { player_system::player = player; }

	static SDL_Point get_player_ids(ITerrain* tc);
	
	
	player_system();
	~player_system();
};

