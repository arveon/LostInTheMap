#include "movement_system.h"

int movement_system::millis_per_tile = 500;
int movement_system::elapsed_walking = 0;
float movement_system::tps = .1f;

void movement_system::move_characters_tick(Space& game_space, int dt, ITerrain* tr)
{
	if (dt > 20)
		return;
	int tilewidth = tr->tile_width;
	for (unsigned int i = 0; i < game_space.objects.size(); i++)
	{
		Entity* character = game_space.objects.at(i);
		//get moving component to see if entity is movable
		IMoving* mc = static_cast<IMoving*>(character->get_component(ComponentType::Movement));

		//if not moving, skip
		if (!mc)
			continue;

		Transform* tc = static_cast<Transform*>(character->get_component(ComponentType::Transf));
		IDrawable* dc = static_cast<IDrawable*>(character->get_component(ComponentType::Drawable));
		
		if (mc->destination_reached)//if destination tile reached (or not moving at all)
			continue;

		SDL_Point cur_dest;
		if (mc->path.size() > 1)
		{
			cur_dest = mc->path.back();
			IDrawable* tile_dc = static_cast<IDrawable*>(tr->terrain_tiles[cur_dest.y][cur_dest.x]->get_component(ComponentType::Drawable));
			cur_dest.x = cur_dest.x * tilewidth - dc->sprite_origin.x + tile_dc->sprite_origin.x;
			cur_dest.y = cur_dest.y * tilewidth - dc->sprite_origin.y + tile_dc->sprite_origin.y;

			if (tc->position.x == cur_dest.x && tc->position.y == cur_dest.y)
			{
				mc->pathfinder.set_origin({ mc->path.back().x, mc->path.back().y });
				mc->path.pop_back();
			}
		}
		else
		{//if still need to move within the tile
			if (!mc->path.empty())
			{
				mc->pathfinder.set_origin({ mc->path.back().x, mc->path.back().y });
				mc->path.pop_back();
			}
			cur_dest.x = mc->final_destination.x;
			cur_dest.y = mc->final_destination.y;
			SDL_Point char_origin = character->get_origin_in_world();
			if (tc->position.x == mc->final_destination.x && tc->position.y == mc->final_destination.y)
			{
				
				tc->position.x = mc->final_destination.x;
				tc->position.y = mc->final_destination.y;
				mc->destination_reached = true;
			}
		}

		int delta_x, delta_y;
		delta_x = cur_dest.x - tc->position.x;
		delta_y = cur_dest.y - tc->position.y;

		double angle = (double)std::atan2(delta_y, delta_x);
		float a = std::cos(angle);
		float b = std::sin(angle);
		float shift_x = a * movement_system::tps*dt;
		float shift_y = b * movement_system::tps*dt;

		//required so lower speeds could be used (buffer accumulates until reached >1
		//if buffer reached whole numbers, it updates the character coordinates and is reduced down to 0.something)
		static float shift_buffer_x = 0, shift_buffer_y = 0;
		shift_buffer_x += shift_x;
		shift_buffer_y += shift_y;
		if (shift_buffer_x > 1.f || shift_buffer_x < -1.f)
		{
			//since floor rounds negative values down towards negative (-0.5 to -1) and ceil rounds them up towards positive (-0.5 to 0)
			//without this if statement speed is larger towards negative 
			if (shift_buffer_x > 0)
				tc->position.x += std::floor(shift_buffer_x);
			else
				tc->position.x += std::ceil(shift_buffer_x);
			//reset shift buffer
			if (shift_buffer_x > 0)
				shift_buffer_x -= 1.f;
			else
				//tc->position.x -= std::ceil(shift_buffer_x);
				shift_buffer_x += 1.f;
		}

		if (shift_buffer_y > 1.f || shift_buffer_y < -1.f)
		{
			if (shift_buffer_y > 0)
				tc->position.y += std::floor(shift_buffer_y);
			else
				tc->position.y += std::ceil(shift_buffer_y);
			//reset shift buffer
			if (shift_buffer_y > 0)
				shift_buffer_y -= 1.f;
			else
				shift_buffer_y += 1.f;
		}
		
	}




}

movement_system::movement_system()
{
}


movement_system::~movement_system()
{
}
