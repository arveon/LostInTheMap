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
		//get moving component to see if entity is movable
		IMoving* mc = static_cast<IMoving*>(game_space.objects.at(i)->get_component(ComponentType::Movement));

		//if not moving, skip
		if (!mc)
			continue;

		Transform* tc = static_cast<Transform*>(game_space.objects.at(i)->get_component(ComponentType::Transf));
		IDrawable* dc = static_cast<IDrawable*>(game_space.objects.at(i)->get_component(ComponentType::Drawable));
		
		if (mc->path.size() == 0)//if destination tile reached (or not moving at all)
			continue;

		elapsed_walking += dt;
		if (elapsed_walking >= millis_per_tile)
		{
			elapsed_walking = 0;
			//update position
			SDL_Point p= mc->path.back();
			//IDrawable* tile_dc = static_cast<IDrawable*>(tr->terrain_tiles[p.y][p.x]->get_component(ComponentType::Drawable));
			//tc->position.x = p.x * tilewidth - dc->sprite_origin.x + tile_dc->sprite_origin.x;
			//tc->position.y = p.y * tilewidth - dc->sprite_origin.y + tile_dc->sprite_origin.y;
			//dc->draw_rect.x = tc->position.x;
			//dc->draw_rect.y = tc->position.y;

			////pop back
			//mc->path.pop_back();

			//mc->pathfinder.set_origin({ p.x, p.y });
		}
		if (mc->path.size() != 0)
		{
			SDL_Point cur_dest = mc->path.back();
			IDrawable* tile_dc = static_cast<IDrawable*>(tr->terrain_tiles[cur_dest.y][cur_dest.x]->get_component(ComponentType::Drawable));
			cur_dest.x = cur_dest.x * tilewidth - dc->sprite_origin.x + tile_dc->sprite_origin.x;
			cur_dest.y = cur_dest.y * tilewidth - dc->sprite_origin.y + tile_dc->sprite_origin.y;

			int delta_x, delta_y;
			delta_x = cur_dest.x - tc->position.x;
			delta_y = cur_dest.y - tc->position.y;

			float angle = (float)std::atan2(delta_y, delta_x);
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
				tc->position.x += std::round(shift_buffer_x);
				shift_buffer_x = 0;
				/*if (shift_buffer_x > 0)
					shift_buffer_x -= 1.f;
				else
					shift_buffer_x += 1.f;*/
			}

			if (shift_buffer_y > 1.f || shift_buffer_y < -1.f)
			{
				tc->position.y += std::round(shift_buffer_y);
				shift_buffer_y = 0;
				/*if (shift_buffer_y > 0)
					shift_buffer_y -= 1.f;
				else
					shift_buffer_y += 1.f;*/
			}


			if (tc->position.x == cur_dest.x && tc->position.y == cur_dest.y)
			{
				mc->pathfinder.set_origin({ mc->path.back().x, mc->path.back().y });
				mc->path.pop_back();
			}
		}


		
	}




}

movement_system::movement_system()
{
}


movement_system::~movement_system()
{
}
