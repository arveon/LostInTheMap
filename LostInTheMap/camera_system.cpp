#include "camera_system.h"

SDL_Rect camera_system::camera_rect;
SDL_Point camera_system::dest_point;
float camera_system::speed;

void camera_system::init_camera()
{	
	camera_rect = SDL_Rect{0,0,0,0};
	SDL_manager::get_window_size(&camera_rect.w, &camera_rect.h);
}

SDL_Rect camera_system::world_to_camera_space(SDL_Rect world_rect)
{
	SDL_Rect result;

	result = { world_rect.x - camera_rect.x ,  world_rect.y - camera_rect.y, world_rect.w, world_rect.h };

	return result;
}

camera_system::camera_system()
{
}


camera_system::~camera_system()
{
}
