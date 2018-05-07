#pragma once
#include <vector>
#include "Entity.h"
class army_system
{
private:
	static std::vector<army_unit*> player_army;

public:
	static void set_player_army(std::vector<army_unit*> pl);
	static std::vector<army_unit*> get_player_army() { return player_army; }

	static void add_to_player_army(army_unit* u) { player_army.push_back(u); }

	static bool is_player_army_initialised() { return player_army.size() != 0; }

	//static void apply_battle_results() { player_army.clear(); }
	army_system();
	~army_system();
};

