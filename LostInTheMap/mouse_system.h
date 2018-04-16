#pragma once
#include "Entity.h"
#include "input_system.h"
#include "SpaceSystem.h"
#include "camera_system.h"

class mouse_system
{
public:
	enum mouse_icons
	{
		normal,
		walking,
		attack,
		talk,
		blocked
	};
	
	static void change_mouse_icon(mouse_icons icon, IAnimatable* anim_component, IDrawable* draw_component);
	static Entity* create_mouse();
	static void update_mouse(Entity* mouse, Space& space, bool in_dialog, bool is_hidden);
	static SDL_Point get_mouse_in_world(Entity* mouse);

	static void update_mouse_hover(Space& space, Entity* mouse);
	mouse_system();
	~mouse_system();

private:
	static mouse_icons cur_icon;
};

