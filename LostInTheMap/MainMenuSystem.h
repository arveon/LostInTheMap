#pragma once
#include "SpaceSystem.h"
#include "Space.h"
#include "MenuLayout.h"
#include "asset_controller.h"

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

	
	static Space load_menu;
	static Space options_menu;
	static Space exit_confirm_dialog;

	static std::vector<void(*)()> exit_listeners;
public:
	//use this for initialisation
	static void init(Space& space, MenuLayout layout, func_reg, func_reg, func_rem);

	//used internally by the system
	static void init_space(Space& space, MenuLayout layout);

	static void update_space(Space& space, int dt);

	static void destroy_space(Space& space);
	
	static void mouse_down_listener() { std::cout << "mouse down" << std::endl; };
	static void mouse_up_listener() { std::cout << "mouse up" << std::endl; };

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

	MainMenuSystem();
	~MainMenuSystem();
};

