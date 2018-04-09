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

#include "geometry_utilities.h"
class game_flow_normal : SpaceSystem
{
private:
	static Entity* mouse;

	static bool lmb_down_event;
	static bool lmb_up_event;
public:
	static int mouse_up_listener_id;
	static int mouse_down_listener_id;
	static void init(Space& game_space);
	static void update_space(Space& space, int dt);
	static void handle_mouse_clicks(Space& space);
	static void update_pathfinder(Space& space);
	static void set_movement(Space& space);

	static void mouse_down_event();
	static void mouse_up_event();
	game_flow_normal();
	~game_flow_normal();
};

