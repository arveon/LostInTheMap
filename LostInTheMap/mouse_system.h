#pragma once
#include "Entity.h"
#include "input_system.h"

class mouse_system
{
public:
	enum mouse_icons
	{
		normal,
		walking
	};

	static void change_mouse_icon(mouse_icons icon, IAnimatable* anim_component, IDrawable* draw_component);
	static Entity* create_mouse();
	static void update_mouse(Entity* mouse);
	mouse_system();
	~mouse_system();
};

