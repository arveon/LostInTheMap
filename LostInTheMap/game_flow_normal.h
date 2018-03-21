#pragma once
#include "Space.h"
#include "SpaceSystem.h"
#include "render_system.h"
#include "camera_system.h"
#include "mouse_system.h"
class game_flow_normal : SpaceSystem
{
private:
	static Entity* mouse;
public:
	static void init(Space& game_space);
	static void update_space(Space& space, int dt);
	game_flow_normal();
	~game_flow_normal();
};

