#include "army_system.h"

std::vector<army_unit*> army_system::player_army;
//std::vector<army_unit*> army_system::battle_results_to_player;

void army_system::set_player_army(std::vector<army_unit*> pl) 
{
	if (player_army.size() > 0)
	{
		for (army_unit* u : player_army)
			delete u;
	}
	player_army = pl; 
}


army_system::army_system()
{
}


army_system::~army_system()
{
}
