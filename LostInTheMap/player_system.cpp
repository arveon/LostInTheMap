#include "player_system.h"

Entity* player_system::player;

SDL_Point player_system::get_player_ids(ITerrain* tc)
{
	SDL_Point player_ids = map_system::world_to_tilemap_ids(player_system::player->get_origin_in_world(), tc);
	return player_ids;
}

player_system::player_system()
{
}


player_system::~player_system()
{
}
