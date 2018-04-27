#pragma once
#include <SDL.h>

///Class responsible for updating and managing game running time and elapsed time
class game_time
{
private:
	Uint32 elapsed_time;
	Uint32 run_time;

public:
	void init() { run_time = SDL_GetTicks(); elapsed_time = 0; }
	void update() 
	{
		elapsed_time = SDL_GetTicks() - run_time;
		run_time += elapsed_time;
	}

	int get_delta_time() { return static_cast<int>(elapsed_time); }
	Uint32 get_run_time() { return run_time; }
};