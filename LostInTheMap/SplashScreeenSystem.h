#pragma once
#include <string>
#include <SDL.h>

#include "asset_controller.h"
#include "Space.h"
#include "Entity.h"

class SplashScreenSystem
{
private:
	
	enum splash_state
	{
		not_initialised,
		appearing,
		full,
		disappearing,
		done
	};
	splash_state state;
public:
	static SDL_Renderer* renderer;
	static void init_space(Space& space);
	static void update(int dt) {};

	SplashScreenSystem();
	~SplashScreenSystem();
};

