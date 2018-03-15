#pragma once
#include "Space.h"
#include "render_system.h"
class game_flow_normal
{
public:
	static void init(Space& game_space);
	static void update_space(Space& space, int dt);
	game_flow_normal();
	~game_flow_normal();
};

