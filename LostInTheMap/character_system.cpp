#include "character_system.h"



std::vector<Entity*> character_system::init_characters(int ** charact, int width, int height, ITerrain* tr)
{
	std::vector<Entity*> characters;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (charact[i][j] != -1)
			{
				Entity* ent = new Entity(entity_type::game_object);

				IDrawable* dc = new IDrawable(ent, IDrawable::layers::background);
				ent->add_component(dc);
				dc->id  = charact[i][j];
				Transform* transf_c = new Transform(ent);
				ent->add_component(transf_c);
				transf_c->position = {
					static_cast<int>(j*tr->tile_width*asset_controller::tile_scaling),
					static_cast<int>(i*tr->tile_width*asset_controller::tile_scaling),
					static_cast<int>(tr->tile_width*asset_controller::tile_scaling),
					static_cast<int>(tr->tile_width*asset_controller::tile_scaling)
				};


				switch (charact[i][j])
				{
				case 1:
					ent->name = "player";
					break;
				default:
					ent->name = "Unnamed";
					break;
				}

				characters.push_back(ent);
			}


		}
	}


	return characters;
}

character_system::character_system()
{
}


character_system::~character_system()
{
}
