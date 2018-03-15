#pragma once
#include <SDL.h>
#include "SDL_manager.h"

class camera_system
{
private:
	static SDL_Rect camera_rect;

	static SDL_Point dest_point;
	static float speed;
public:
	static void init_camera();

	static SDL_Rect world_to_camera_space(SDL_Rect world_rect);

	static void move_camera_to(SDL_Point destination);
	static void update_animation(int dt);
	camera_system();
	~camera_system();
};

