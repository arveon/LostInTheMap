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

	int t_total_time = 1000;
	int t_elapsed_time = 0;
	
	int loading_progress = 0;
public:
	
	static void load_new_game();

	static void init_space(MenuLayout layout, Space& space);
	static void update_space(Space& space, Space& level_space);
	

	level_loading_system();
	~level_loading_system();
};

