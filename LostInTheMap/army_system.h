#pragma once
#include <vector>

#include "Entity.h"
#include "SpaceSystem.h"

class army_system
{
private:
	static std::vector<army_unit*> player_army;
	static std::vector<Entity*> unit_frames;

	static SDL_Point panel;
	static int portrait_width;
	
public:
	static int a;
	static void set_player_army(std::vector<army_unit*> pl);
	static void init_player_army_frames(Space * game_space);
	static std::vector<army_unit*> get_player_army() { return player_army; }

	static void add_to_player_army(army_unit* u, Space* game_space);

	static Entity * create_unit_frame(army_unit * u);

	static void update_frame_quantity(Entity * frame);

	static void update_unit_frames();

	static bool is_player_army_initialised() { return player_army.size() != 0; }

	static void reset_army_system() { unit_frames.clear(); a = 0; player_army.clear(); }

	//static void apply_battle_results() { player_army.clear(); }
	army_system();
	~army_system();
};

