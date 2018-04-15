#include "camera_system.h"

SDL_Rect camera_system::camera_rect;
SDL_Point camera_system::dest_point;
float camera_system::speed;

Entity* camera_system::target;
bool camera_system::snapped = true;

float camera_system::zoom = 1.f;
float camera_system::shift_buffer_x= 0;
float camera_system::shift_buffer_y = 0;

SDL_Point camera_system::total_distance_to_object;

int camera_system::gridwidth;
int camera_system::gridheight;
int camera_system::tilewidth;

void(*camera_system::camera_reached_dest_callback)(Entity*);

void camera_system::init_camera(int tilewidth, Entity* target)
{	
	camera_system::target = target;
	zoom = 1.f;
	speed = .2f;
	camera_rect = SDL_Rect{0,0,0,0};
	SDL_manager::get_window_size(&camera_rect.w, &camera_rect.h);

	gridwidth = camera_rect.w / tilewidth;
	gridheight = camera_rect.h / tilewidth;
	camera_system::tilewidth = tilewidth;
	shift_buffer_x = 0;
	shift_buffer_y = 0;
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

void camera_system::set_camera_target(Entity * new_target, bool snap, void(*callback)(Entity *))
{
	snapped = snap; 
	target = new_target; 
	if (callback != nullptr)
		camera_reached_dest_callback = callback;

	//calculate distance
	SDL_Point target_origin = target->get_origin_in_world();
	int required_delta_x, required_delta_y;
	total_distance_to_object.x = target_origin.x - camera_rect.w/2 - camera_rect.x;
	total_distance_to_object.y = target_origin.y - camera_rect.h/2 - camera_rect.y;
}

void camera_system::update_camera(int dt)
{
	if (snapped)
	{
		Transform* tc = static_cast<Transform*>(target->get_component(Component::ComponentType::Transf));
		if (!tc)
			return;
		camera_rect.x = tc->position.x - camera_rect.w / 2 + tc->origin.x;
		camera_rect.y = tc->position.y - camera_rect.h / 2 + tc->origin.y;
	}
	else
	{
		if (dt > 20)
			return;
		//calculate distance to the desired position
		Transform* tc = static_cast<Transform*>(target->get_component(Component::ComponentType::Transf));
		if (!tc)
			return;




		SDL_Point cur_origin;
		cur_origin.x = camera_rect.x + camera_rect.w / 2;
		cur_origin.y = camera_rect.y + camera_rect.h / 2;
		SDL_Point target_origin = target->get_origin_in_world();

		if (cur_origin.x == target_origin.x && cur_origin.y == target_origin.y)
		{
			if (camera_reached_dest_callback)
				camera_reached_dest_callback(nullptr);
			snapped = true;
			return;
		}
		
		SDL_Point destination;
		destination.x = target_origin.x - camera_rect.w / 2;
		destination.y = target_origin.y - camera_rect.h / 2;

		int required_delta_x, required_delta_y;
		required_delta_x = destination.x - camera_rect.x;
		required_delta_y = destination.y - camera_rect.y;

		//this block makes camera speed increase and decrease gradually
		//calculate % reached
		float px, py;
		px = 1.f - std::fabs((float)(required_delta_x - ((float)total_distance_to_object.x / 2)) / ((float)total_distance_to_object.x / 2));
		py = 1.f - std::fabs((float)(required_delta_y - ((float)total_distance_to_object.y / 2)) / ((float)total_distance_to_object.y / 2));
		float cur_speed_x = speed * px + 0.01f;
		float cur_speed_y = speed * py + 0.01f;

		//calculate the speed depending on remaining distance to desired position
		float tick_delta_x, tick_delta_y;
		float angle = std::atan2(required_delta_y, required_delta_x);
		tick_delta_x = std::cos(angle) * cur_speed_x * dt;
		tick_delta_y = std::sin(angle) * cur_speed_y * dt;

		//update camera position depending on speed
		shift_buffer_x += tick_delta_x;
		shift_buffer_y += tick_delta_y;

		if (shift_buffer_x > 1)
		{
			camera_rect.x += 1;
			shift_buffer_x -= 1;
		}
		else if (shift_buffer_x < -1)
		{
			camera_rect.x -= 1;
			shift_buffer_x += 1;
		}

		if (shift_buffer_y > 1)
		{
			camera_rect.y += 1;
			shift_buffer_y -= 1;
		}
		else if (shift_buffer_y < -1)
		{
			camera_rect.y -= 1;
			shift_buffer_y += 1;
		}
	}

	
}

void camera_system::set_camera_zoom(float new_zoom)
{
	int window_w, window_h;
	SDL_manager::get_window_size(&window_w, &window_h);

	camera_rect.w = (int)((float)window_w / new_zoom);
	camera_rect.h = (int)((float)window_h / new_zoom);

	zoom = new_zoom;
}

SDL_Rect camera_system::get_camera_rect_ids(int tilewidth)
{
	SDL_Rect ids;
	ids.x = (int)std::ceil(camera_system::camera_rect.x / tilewidth);
	ids.y = (int)std::ceil(camera_system::camera_rect.y / tilewidth);
	ids.w = (int)std::ceil(camera_system::camera_rect.w / tilewidth);
	ids.h = (int)std::ceil(camera_system::camera_rect.h / tilewidth);
	return ids;
}

camera_system::camera_system()
{
}

camera_system::~camera_system()
{
}
