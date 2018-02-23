#pragma once
#include "Space.h"
#include "render_system.h"
#include "Components.h"

class SpaceSystem
{
public:
	SpaceSystem() {};
	~SpaceSystem() {};
	virtual void init_space(Space& space) = 0;
	virtual void update_space(Space& space, int delta_time) = 0;
	virtual void destroy_space(Space& space) = 0;
	static void add_space_to_render_queue(Space& space);//function will add all of the drawable components of objects in spaces to render manager

	static Component* find_component_on_object(Entity*, ComponentType);
};

