#include "game_flow_normal.h"

void game_flow_normal::init(Space & game_space)
{
	for (unsigned int i = 0; i < game_space.objects.size(); i++)
	{
		Entity* ent = game_space.objects.at(i);
		IDrawable* drawable = static_cast<IDrawable*>(ent->get_component(ComponentType::Transf));
		
		ITerrain* tr = static_cast<ITerrain*>(ent->get_component(ComponentType::Terrain));
		if (tr)
		{
			
			for (int i = 0; i < tr->height; i++)
				for (int j = 0; j < tr->width; j++)
				{
					IDrawable* dc = static_cast<IDrawable*>(tr->terrain_tiles[i][j]->get_component(ComponentType::Drawable));
					render_system::add_object_to_queue(dc);
				}
		}

		if (!drawable)
			continue;
		render_system::add_object_to_queue(drawable);
	}

	game_space.initialised = true;
}

void game_flow_normal::update_space(Space & space, int dt)
{
}

game_flow_normal::game_flow_normal()
{
}


game_flow_normal::~game_flow_normal()
{
}
