#pragma once

#include "MenuSystem.h"
#include "SpaceSystem.h"


class PauseMenuSystem
{
private:
	static Space menu_space;
	static Entity* mouse;

	static int down_listener_id;
	static int up_listener_id;
public:
	static void(*resume)();
	static void(*back_to_menu)();

	static bool is_menu_on() { return menu_space.initialised; };
	static void init_space(Entity* mouse);
	static void resume_game();
	static void return_to_menu();
	static void close_menu();

	static void resume_callback() { resume(); }
	static void back_to_menu_callback() { back_to_menu(); }

	static void update_menu(int dt);

	PauseMenuSystem();
	~PauseMenuSystem();
};

