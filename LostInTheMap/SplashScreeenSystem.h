#pragma once
#include <string>
#include <SDL.h>
#include <vector>

#include "asset_controller.h"
#include "Space.h"
#include "Entity.h"
#include "SpaceSystem.h"
#include "asset_controller.h"

class SplashScreenSystem : public SpaceSystem
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
	static splash_state state;

	static int cur_alpha;
	static const int total_stage_time = 2000;
	static int elapsed_stage_time;

	static std::vector<void(*)()> listeners;
public:
	static void init_space(Space& space);
	static void register_splash_elapsed_listener(void(*callback)()) { listeners.push_back(callback); }
	static void update_space(Space& space, int dt);
	
	static void destroy_space(Space& space);

	SplashScreenSystem();
	~SplashScreenSystem();
};

