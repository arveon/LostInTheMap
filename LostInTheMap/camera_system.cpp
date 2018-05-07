#include "camera_system.h"

SDL_Rect camera_system::camera_rect;
SDL_Point camera_system::dest_point;
float camera_system::speed = .3f;
float camera_system::speed_breakpoint = .2f;

Entity* camera_system::target;
bool camera_system::snapped = true;

float camera_system::zoom = 1.f;
float camera_system::shift_buffer_x= 0;
float camera_system::shift_buffer_y = 0;

SDL_Point camera_system::total_distance_to_object;

int camera_system::gridwidth;
int camera_system::gridheight;
int camera_system::tilewidth;

int camera_system::x_cap;
int camera_system::y_cap;

void(*camera_system::camera_reached_dest_callback)(Entity*);

void camera_system::init_camera(int tilewidth, int level_width, int level_height, Entity* target)
{	
	camera_system::target = target;
	zoom = 1.f;
	camera_rect = SDL_Rect{0,0,0,0};
	SDL_manager::get_window_size(&camera_rect.w, &camera_rect.h);

	x_cap = level_width;
	y_cap = level_height;

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
	if (new_target == nullptr)
	{
		target = nullptr;
		return;
	}
	
	snapped = snap; 
	target = new_target; 
	if (callback != nullptr)
		camera_reached_dest_callback = callback;

	//calculate distance
	SDL_Point target_origin = target->get_origin_in_world();
	total_distance_to_object.x = target_origin.x - camera_rect.w/2 - camera_rect.x;
	total_distance_to_object.y = target_origin.y - camera_rect.h/2 - camera_rect.y;
}

void camera_system::update_camera(int dt)
{
	if (snapped)
	{
		if (!target)
			return;
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
		if (target_origin.y + camera_rect.h / 2 > y_cap)
			target_origin.y = y_cap-camera_rect.h;

		if (cur_origin.x == target_origin.x && cur_origin.y == target_origin.y)
		{
			snapped = true;
			if (camera_reached_dest_callback)
				camera_reached_dest_callback(nullptr);
			return;
		}
		
		SDL_Point destination;
		destination.x = target_origin.x - camera_rect.w / 2;
		destination.y = target_origin.y - camera_rect.h / 2;

		int required_delta_x, required_delta_y;
		required_delta_x = destination.x - camera_rect.x;
		required_delta_y = destination.y - camera_rect.y;

		//make camera increase speed and decrease speed depending on how much of path was traversed
#pragma region camera_smoothing
		float cur_speed_x, cur_speed_y;
		SDL_Point trav_d = {std::abs(total_distance_to_object.x - required_delta_x), std::abs(total_distance_to_object.y - required_delta_y) };

		if (trav_d.x < std::abs(total_distance_to_object.x)*speed_breakpoint)
			cur_speed_x = std::abs(trav_d.x / (speed_breakpoint * total_distance_to_object.x));
		else if (trav_d.x > std::abs(total_distance_to_object.x) * (1.f- speed_breakpoint))
			cur_speed_x = std::abs((std::abs(total_distance_to_object.x) - trav_d.x) / (speed_breakpoint * total_distance_to_object.x));
		else
			cur_speed_x = 1.f;

		if (trav_d.y < std::abs(total_distance_to_object.y)*speed_breakpoint)
			cur_speed_y = std::abs(trav_d.y / (speed_breakpoint * total_distance_to_object.y));
		else if (trav_d.y > std::abs(total_distance_to_object.y) * (1.f - speed_breakpoint))
			cur_speed_y = std::abs((std::abs(total_distance_to_object.y) - trav_d.y) / (speed_breakpoint * total_distance_to_object.y));
		else
			cur_speed_y = 1.f;

		if (total_distance_to_object.x == 0)
			cur_speed_x = 0;
		else
			cur_speed_x = (cur_speed_x == 0) ? 0.1f : cur_speed_x * speed;

		if(total_distance_to_object.y == 0)
			cur_speed_y = 0;
		else
			cur_speed_y = (cur_speed_y != 0) ? cur_speed_y * speed : 0.1f;
#pragma endregion

		//calculate the speed depending on remaining distance to desired position
		float tick_delta_x, tick_delta_y;
		float angle = (float)std::atan2(required_delta_y, required_delta_x);
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

	if (camera_rect.x < 0)
		camera_rect.x = 0;
	else if (camera_rect.x + camera_rect.w > x_cap-1)
		camera_rect.x = x_cap - camera_rect.w -1;

	if (camera_rect.y < 0)
		camera_rect.y = 0;
	else if (camera_rect.y + camera_rect.h > y_cap-1)
		camera_rect.y = y_cap - camera_rect.h -1;
}


void camera_system::set_camera_zoom(float new_zoom)
{
	int window_w, window_h;
	SDL_manager::get_window_size(&window_w, &window_h);

	camera_rect.w = (int)((float)window_w / new_zoom);
	camera_rect.h = (int)((float)window_h / new_zoom);

	zoom = new_zoom;
}

SDL_Rect camera_system::get_camera_rect_ids(int tilewidth, int tileheight)
{
	SDL_Rect ids;
	ids.x = (int)std::ceil(camera_system::camera_rect.x / tilewidth);
	ids.y = (int)std::ceil(camera_system::camera_rect.y / tileheight);
	ids.w = (int)std::ceil(camera_system::camera_rect.w / tilewidth);
	ids.h = (int)std::ceil(camera_system::camera_rect.h / tileheight);
	return ids;
}

camera_system::camera_system()
{
}

camera_system::~camera_system()
{
}
