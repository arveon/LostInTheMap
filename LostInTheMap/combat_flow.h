#pragma once
#include <string>

#include "Levels.h"
#include "xml_system.h"
#include "asset_controller.h"
#include "map_system.h"
#include "mouse_system.h"
#include "movement_system.h"


class combat_flow
{
private:
	static bool combat_finished;
	static bool initialised;
	
	static Entity* mouse;
	static int cur_turn;

	static std::vector<army_unit*> order_of_turns;
public:
	static Space combat_space;
	static std::vector<army_unit> player_army;
	static std::vector<army_unit> enemy_army;

	static void init_player_army(std::vector<army_unit> army);
	static void init_enemy_army(std::vector<army_unit> army);
	static void init_combat_space(Space& game_space);
	static void destroy_combat(Space& game_space);

	static void update(Space& game_space, int dt);

	static bool is_initialised() { return initialised; }
	static bool is_in_combat() { return !combat_finished; }
	static bool is_player_army_initialised() { return player_army.size() != 0; }

	static void set_in_combat() { combat_finished = false; }

	static void compose_turn_orders();

	static void mouse_clicked();

	static void unit_finished_moving(Entity* unit);
	static void unit_finished_turn();

	static void combat_round_finished();

	combat_flow();
	~combat_flow();
};

