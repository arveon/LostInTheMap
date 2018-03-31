#include "camera_system.h"

SDL_Rect camera_system::camera_rect;
SDL_Point camera_system::dest_point;
float camera_system::speed;

Entity* camera_system::target;
bool camera_system::snapped = true;

float camera_system::zoom = 1.f;

void camera_system::init_camera()
{	
	zoom = 1.f;
	camera_rect = SDL_Rect{0,0,0,0};
	SDL_manager::get_window_size(&camera_rect.w, &camera_rect.h);
}

SDL_Rect camera_system::world_to_camera_space(SDL_Rect world_rect, SDL_Rect draw_rect)
{
	SDL_Rect result;

	result = { world_rect.x - camera_rect.x ,  world_rect.y - camera_rect.y, static_cast<int>(draw_rect.w*zoom), static_cast<int>(draw_rect.h*zoom) };

	return result;
}

SDL_Point camera_system::screen_to_world_space(SDL_Point position)
{
	SDL_Point result = {0,0};
	//required to position/zoom to get position to camera coordinates from screen coordinates
	result = { camera_rect.x + static_cast<int>(position.x/(float)zoom), camera_rect.y + static_cast<int>(position.y / (float)zoom) };

	return result;
}

void camera_system::update_camera()
{
	if (snapped)
	{
		Transform* tc = static_cast<Transform*>(target->get_component(Component::ComponentType::Transf));
		camera_rect.x = tc->position.x - camera_rect.w / 2 + tc->position.w / 2;
		camera_rect.y = tc->position.y - camera_rect.h / 2 + tc->position.h / 2;
	}
	else
	{
		//calculate distance to the desired position

		//calculate the speed depending on remaining distance to desired position

		//update camera position depending on speed
	}

	
}

void camera_system::set_camera_zoom(float new_zoom)
{
	int window_w, window_h;
	SDL_manager::get_window_size(&window_w, &window_h);

	camera_rect.w = window_w / new_zoom;
	camera_rect.h = window_h / new_zoom;
	zoom = new_zoom;
}

camera_system::camera_system()
{
}

camera_system::~camera_system()
{
}
