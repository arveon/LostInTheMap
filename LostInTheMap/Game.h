#pragma once
#include "SDL_manager.h"
#include "input_system.h"
#include "xml_system.h"
#include "asset_controller.h"
#include "SplashScreeenSystem.h"
#include "MainMenuSystem.h"
#include "level_loading_system.h"
#include "EventTypes.h"
#include "Time.h"
#include "Components.h"

#include <stdlib.h>



enum game_state
{
	splash,
	main_menu,
	loading,
	pause_menu,
	game_flow,
	confirming_exit,
	exit_confirmed
};

//generic game object that will contain initialisation logic and the game loop methods
class Game
{
private:
	static game_state state;
	SDL_manager sdl_manager;
	static bool running;

	time time;
	static Space splash_screen_space;
	static Space main_menu_space;
	static Space game_space;
	static Space pause_menu_space;
	static Space loading_space;

public:
	Game();
	~Game();
	
	void game_loop();
	void init();
	void init_splash();
	void finish();
	static void window_close_handler() { running = false; }
	static void loading_done_handler();
	static void splash_elapsed_handler();
	static void start_handler();

	//function will be passed to initialise function that would allow them to register mouseclick callback methods
	static int register_mousedown_listener(void(*cb)())
	{
		return input_system::register_event_callback(HardInputEventType::left_mouse_down, cb);
	}

	//function will be passed to initialise function that would allow them to register mouseclick callback methods
	static int register_mouseup_listener(void(*cb)())
	{
		return input_system::register_event_callback(HardInputEventType::left_mouse_up, cb);
	}
	
	static void deregister_event_listener(HardInputEventType type,int id)
	{
		bool result = input_system::remove_event_callback(type, id);
	}
};

