#pragma once
#include "SDL_manager.h"
#include "input_system.h"
#include "xml_system.h"

#include <stdlib.h>
#include "EventTypes.h"


//generic game object that will contain initialisation logic and the game loop methods
class Game
{
private:
	SDL_manager sdl_manager;
	static bool running;
public:
	Game();
	~Game();
	
	void game_loop();
	void init();
	void finish();
	static void window_close() { running = false; };
};

