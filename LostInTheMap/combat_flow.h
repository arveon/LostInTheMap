#pragma once
#include <string>

#include "Levels.h"
#include "xml_system.h"
#include "asset_controller.h"
#include "map_system.h"
#include "mouse_system.h"


class combat_flow
{
private:
	static bool combat_finished;
	static bool initialised;
	static Space combat_space;
	static Entity* mouse;
public:
	static std::vector<army_unit> player_army;

	static void init_player_army(std::vector<army_unit> army);
	static void init_combat_space(Space& game_space);
	static void destroy_combat(Space& game_space);

	static void update(Space& game_space, int dt);

	static bool is_initialised() { return initialised; }
	static bool is_in_combat() { return !combat_finished; }
	static bool is_player_army_initialised() { return player_army.size() != 0; }

	static void set_in_combat() { combat_finished = false; }



	combat_flow();
	~combat_flow();
};

