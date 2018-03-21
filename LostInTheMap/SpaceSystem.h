#pragma once
#include "Space.h"
#include "render_system.h"
#include "Components.h"
#include "asset_controller.h"
#include "input_system.h"

class SpaceSystem
{
public:
	SpaceSystem() {};
	~SpaceSystem() {};
	virtual void init_space(Space& space) = 0;
	virtual void update_space(Space& space, int delta_time) = 0;
	//virtual void handle_mouse() {};
	static void add_space_to_render_queue(Space& space);//function will add all of the drawable components of objects in spaces to render manager
	static void apply_animation_sprite_changes(Space& space);
	static void destroy_space(Space& space);
	
	static Entity* find_entity_by_name(Space& space, std::string name);
	
};

