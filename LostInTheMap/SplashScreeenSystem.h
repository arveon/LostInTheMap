#pragma once
#include <string>
#include "asset_controller.h"
#include "Space.h"
#include "Entity.h"

class SplashScreenSystem
{
private:
	
	enum splash_state
	{
		appearing,
		full,
		disappearing,
		done
	};
	splash_state state;
public:
	static SDL_Renderer* renderer;
	static void init_space(Space& space);

	SplashScreenSystem();
	~SplashScreenSystem();
};

