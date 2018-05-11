#pragma once
#include "Space.h"
#include "SpaceSystem.h"
#include "render_system.h"
#include "camera_system.h"
#include "mouse_system.h"
#include "movement_system.h"
#include "map_system.h"
#include "player_system.h"
#include "character_system.h"
#include "dialogue_system.h"
#include "director.h"
#include "combat_flow.h"
#include "animator.h"
#include "PauseMenuSystem.h"

#include "geometry_utilities.h"
class game_flow_normal : public SpaceSystem
{
private:
	static void(*change_level_callback)(levels level);
	static Entity* mouse;

	static bool lmb_down_event;
	static bool lmb_up_event;
public:
	static void(*return_to_menu)();

	static int mouse_up_listener_id;
	static int mouse_down_listener_id;
	static void init(Space& game_space, void(*change_level_cb)(levels));
	static void update_space(Space& space, int dt);

	static void apply_combat_results(Space & space);
	static void handle_mouse_clicks(Space& space);
	static void update_pathfinder(Space& space);
	static void set_movement(Space& space);

	static void clear_all_systems(Space& space);

	static void trigger_pause();
	static void mouse_down_event();
	static void mouse_up_event();
	game_flow_normal();
	~game_flow_normal();
};

