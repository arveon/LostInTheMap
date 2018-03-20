#include "camera_system.h"

SDL_Rect camera_system::camera_rect;
SDL_Point camera_system::dest_point;
float camera_system::speed;

Entity* camera_system::target;
bool camera_system::snapped = true;

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

void camera_system::update_camera()
{
	if (snapped)
	{
		Transform* tc = static_cast<Transform*>(target->get_component(ComponentType::Transf));
		camera_rect.x = tc->position.x - camera_rect.w / 2;
		camera_rect.y = tc->position.y - camera_rect.h / 2;
	}
	else
	{
		//calculate distance to the desired position

		//calculate the speed depending on remaining distance to desired position

		//update camera position depending on speed
	}

	
}

void camera_system::get_camera_zoom(float* w_zoom, float* h_zoom)
{
	int window_w, window_h;
	SDL_manager::get_window_size(&window_w, &window_h);

	*w_zoom = (float)camera_rect.w / (float)window_w;
	*h_zoom = (float)camera_rect.h / (float)window_h;
}

camera_system::camera_system()
{
}

camera_system::~camera_system()
{
}
