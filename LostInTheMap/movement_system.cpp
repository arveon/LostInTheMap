#include "movement_system.h"

int movement_system::millis_per_tile = 500;
int movement_system::elapsed_walking = 0;
float movement_system::tps = .5f;

void movement_system::move_characters_tick(Space& game_space, int dt, ITerrain* tr)
{
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
			IDrawable* tile_dc = static_cast<IDrawable*>(tr->terrain_tiles[p.y][p.x]->get_component(ComponentType::Drawable));
			tc->position.x = p.x * tilewidth - dc->sprite_origin.x + tile_dc->sprite_origin.x;
			tc->position.y = p.y * tilewidth - dc->sprite_origin.y + tile_dc->sprite_origin.y;
			dc->draw_rect.x = tc->position.x;
			dc->draw_rect.y = tc->position.y;

			//pop back
			mc->path.pop_back();

			mc->pathfinder.set_origin({ p.x, p.y });
		}

		/*SDL_Point cur_dest = mc->path.back();
		cur_dest.x *= tilewidth;
		cur_dest.y *= tilewidth;
		
		int delta_x, delta_y;
		delta_x = cur_dest.x - tc->position.x;
		delta_y = cur_dest.y - tc->position.y;
		
		float rotation = (float)std::atan2(delta_y, delta_x);
		float shift_x = std::ceil(std::cos(rotation) * movement_system::tps);
		float shift_y = std::sin(rotation) * movement_system::tps;

		tc->position.x += shift_x;
		tc->position.y += shift_y;*/


		
	}




}

movement_system::movement_system()
{
}


movement_system::~movement_system()
{
}
