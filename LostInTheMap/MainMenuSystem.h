#pragma once
#include "SpaceSystem.h"
#include "Space.h"
#include "MenuLayout.h"
#include "asset_controller.h"
#include "input_system.h"

#include "sdl_utils.h"

#include "EventTypes.h"

#include <iostream>

class MainMenuSystem : public SpaceSystem
{
private:
	typedef int(*func_reg)(void(*)());
	typedef void(*func_rem)(HardInputEventType, int);

	static func_rem dereg_listener;//should be used in destructor to deregister listeners before destroying the object
	static int mouse_down_listener_id;
	static int mouse_up_listener_id;

	static Space gameflow;
	static Space load_screen;
	static Space load_menu;
	static Space options_menu;
	static Space exit_confirm_dialog;

	static std::vector<void(*)()> exit_listeners;

	static Entity* mouse;
public:
	//use this for initialisation from outside
	static void init(Space& space, MenuLayout layout, func_reg, func_reg, func_rem);

	//INTERNAL TO THE SYSTEM, USE init() INSTEAD!
	static void init_space(Space& space, MenuLayout layout);

	static void update_space(Space& space, int dt);
	static void destroy_space(Space& space);
	
	static void mouse_down_listener();
	static void mouse_up_listener();

	static int register_exit_listener(void(*listener)()) { exit_listeners.push_back(listener); return (exit_listeners.size() - 1); }
	static bool deregister_exit_listener(int listener_id) 
	{
		if (listener_id <= (static_cast<int>(exit_listeners.size()) - 1))
		{
			exit_listeners.erase(exit_listeners.begin() + listener_id); return true;
		}
		else
			return false;
	};

	static void mouse_target_changed(Entity* new_target);
	static void mouse_clicked_on_entity();
	static void update_composite_entity(IComposite* composite_entity);
	

	MainMenuSystem();
	~MainMenuSystem();
};

