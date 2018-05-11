#pragma once
#include "MenuLayout.h"
#include "SpaceSystem.h"
#include "mouse_system.h"
#include "sdl_utils.h"

enum menu_type
{
	main_menu,
	pause_menu
};

class MenuSystem
{
public:
	static void(*exit)();
	static void(*start)();
	static void(*resume)();
	static void(*back_to_menu)();

	static Entity * mouse;
	static void init_menu(MenuLayout layout, Space& space, menu_type type);

	static void update_menu_space(Space& space, int dt, Entity* mouse);
	static void mouse_target_changed(Entity * new_target, Entity* mouse);
	static void mouse_down_listener();
	static void mouse_up_listener();
	static void mouse_clicked_on_entity();
	MenuSystem();
	~MenuSystem();
};

