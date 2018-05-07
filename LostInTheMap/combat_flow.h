#pragma once
#include <string>
#include <time.h>
#include <random>

#include "Levels.h"
#include "xml_system.h"
#include "asset_controller.h"
#include "map_system.h"
#include "mouse_system.h"
#include "movement_system.h"
#include "animator.h"
#include "ai_system.h"
#include "army_system.h"


class combat_flow
{
private:
	static bool combat_finished;
	static bool initialised;

	static bool combat_started;
	
	static Entity* mouse;
	static int cur_turn;

	static std::vector<army_unit*> order_of_turns;
public:
	static Space combat_space;
	//static std::vector<army_unit*> player_army;
	static std::vector<army_unit*> enemy_army;
	static bool player_is_winner;

	static void init_player_army(std::vector<army_unit*> army);
	static void init_enemy_army(std::vector<army_unit*> army);
	static void init_combat_space(Space& game_space);
	static void start(Entity* fade);
	static void finish(Entity * fade);
	static void destroy_combat(Space& game_space);

	static void update(Space& game_space, int dt);

	static bool is_initialised() { return initialised; }
	static bool is_in_combat() { return !combat_finished; }

	static void set_in_combat() { combat_finished = false; }

	static void compose_turn_orders();

	static void mouse_clicked();

	static void unit_attacks(Entity * source, Entity * target);

	static void attack_animation_finished_callback(Entity * source);

	static void update_quantity_display(Entity * target);

	static void damaged_animation_finished_callback(Entity * source);

	static void unit_finished_moving(Entity* unit);
	static void unit_finished_turn();

	static bool check_combat_finished();

	static void combat_round_finished();

	combat_flow();
	~combat_flow();
};

