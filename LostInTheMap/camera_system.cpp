#include "camera_system.h"

SDL_Rect camera_system::camera_rect;
SDL_Point camera_system::dest_point;
float camera_system::speed;

Entity* camera_system::target;
bool camera_system::snapped = true;

float camera_system::zoom = 1.f;

int camera_system::gridwidth;
int camera_system::gridheight;
int camera_system::tilewidth;

void camera_system::init_camera(int tilewidth, Entity* target)
{	
	camera_system::target = target;
	zoom = 1.f;
	camera_rect = SDL_Rect{0,0,0,0};
	SDL_manager::get_window_size(&camera_rect.w, &camera_rect.h);

	gridwidth = camera_rect.w / tilewidth;
	gridheight = camera_rect.h / tilewidth;
	camera_system::tilewidth = tilewidth;
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

SDL_Rect camera_system::get_camera_rect_ids(int tilewidth)
{
	SDL_Rect ids;
	ids.x = std::ceil(camera_system::camera_rect.x / tilewidth);
	ids.y = std::ceil(camera_system::camera_rect.y / tilewidth);
	ids.w = std::ceil(camera_system::camera_rect.w / tilewidth);
	ids.h = std::ceil(camera_system::camera_rect.h / tilewidth);
	return ids;
}

camera_system::camera_system()
{
}

camera_system::~camera_system()
{
}
