#pragma once
#include "SDL_manager.h"
#include "input_system.h"
#include "xml_system.h"
#include "asset_controller.h"
#include "SplashScreeenSystem.h"
#include "EventTypes.h"
#include "Time.h"

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
	Space splash_screen_space;
public:
	Game();
	~Game();
	
	void game_loop();
	void init();
	void init_splash();
	void finish();
	static void window_close_handler() { running = false; };
	static void splash_elapsed_handler() { state = game_state::main_menu; };
};

