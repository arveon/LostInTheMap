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

	static float zoom;
public:
	static void init_camera();

	static SDL_Rect world_to_camera_space(SDL_Rect world_rect);
	static SDL_Point screen_to_world_space(SDL_Point position);

	static void move_camera_to(SDL_Point destination);
	static void set_camera_target(Entity* new_target) { target = new_target; }

	static float get_camera_zoom() { return zoom; }
	static void set_camera_zoom(float zoom);
	static SDL_Rect get_camera_rect() { return camera_rect; }
	
	static void update_camera();
	camera_system();
	~camera_system();

	static void print_camera_position() 
	{ 
		std::cout << camera_rect.x << " " << camera_rect.y << std::endl; 
		//std::cout << (camera_rect.x + 192) / 32.f << " " << (camera_rect.y + 160) / 32.f << std::endl;
	}
};

