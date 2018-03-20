#pragma once
#include <SDL.h>
#include "SDL_manager.h"
#include "Entity.h"
#include "render_system.h"

class camera_system
{
private:
	static Entity* target;

	static SDL_Rect camera_rect;
	static SDL_Point dest_point;
	static float speed;

	static bool snapped;
public:
	static void init_camera();

	static SDL_Rect world_to_camera_space(SDL_Rect world_rect);

	static void move_camera_to(SDL_Point destination);
	static void set_camera_target(Entity* new_target) { target = new_target; }

	static void get_camera_zoom(float* w_zoom, float* h_zoom);
	static SDL_Rect get_camera_rect() { return camera_rect; }
	
	static void update_camera();
	camera_system();
	~camera_system();
};

