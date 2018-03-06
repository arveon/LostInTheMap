#pragma once
#include "Space.h"
#include "SpaceSystem.h"
#include "MenuLayout.h"

class level_loading_system : public SpaceSystem
{
private:
	enum loading_state
	{
		loading_terrain,
		creating_terrain_collisions,
		loading_objects,
		loading_characters,
		loading_terrain_textures,
		loading_character_textures,
		loading_item_textures,
		attaching_terrain_textures,
		attaching_character_textures,
		attaching_object_textures,
		cleaning_up_tilesheet,//?
		linking_tiles,
		initialising_pathfinding,
		loading_misc,
		done
	};
	
	static const int t_total_time = 500;
	static int t_elapsed_time;
	
	static loading_state loading_stage;
	static int loading_progress;
	static std::vector<void(*)()> loading_done_listeners;

	static void loading_done();
public:
	
	static void load_new_game();

	static void init_space(MenuLayout layout, Space& space);
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

