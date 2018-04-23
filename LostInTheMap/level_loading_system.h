#pragma once
#include "Space.h"
#include "SpaceSystem.h"
#include "MenuLayout.h"
#include "xml_system.h"
#include "map_system.h"
#include "camera_system.h"
#include "character_system.h"
#include "director.h"
#include "NameToTypeConversion.h"
#include "combat_flow.h"

class level_loading_system : public SpaceSystem
{
private:
	static const int terrain_texture_width = 32;
	static const int terrain_texture_height = 32;
	enum loading_state
	{
		loading_terrain,
		creating_terrain_collisions,
		initialising_pathfinding,
		loading_objects,
		loading_characters,
		loading_terrain_textures,
		loading_character_textures,
		loading_object_textures,
		attaching_terrain_textures,
		attaching_character_textures,
		attaching_object_textures,
		cleaning_up_tilesheet,//?
		loading_misc,
		objects_camera_positions,
		done
	};

	static levels level_to_load;
	
	static const int t_total_time = 100;
	static int t_elapsed_time;
	
	static loading_state loading_stage;
	static int loading_progress;
	static std::vector<void(*)()> loading_done_listeners;

	static std::vector <xml_system::LoadingState> loading_states;

	static void loading_done();

	static void update_bar_fill(Space& space);
	static void update_status_text(Space& space, int state_id);
	static void load_game_components(Space& game_space);
	static void load_combat(levels level, Space & game_space, IFightable*);
public:
	
	static void load_new_game();

	static void init_space(MenuLayout layout, Space& space, levels to_load);
	static void update_space(Space& space, Space& level_space, int dt);
	
	static int register_loading_done_listener(void(*listener)()) { loading_done_listeners.push_back(listener); return (loading_done_listeners.size() - 1); }
	static bool deregister_loading_done_listener(int listener_id)
	{
		if (listener_id <= (static_cast<int>(loading_done_listeners.size()) - 1))
		{
			loading_done_listeners.erase(loading_done_listeners.begin() + listener_id); return true;
		}
		else
			return false;
	};

	static void remove_listeners()
	{
		loading_done_listeners.clear();
	}

	level_loading_system();
	~level_loading_system();
};

