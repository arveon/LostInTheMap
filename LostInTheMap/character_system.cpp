#include "character_system.h"

std::vector<Entity*> character_system::characters;

std::vector<Entity*> character_system::init_characters(int ** charact, int width, int height, ITerrain* tr)
{
	std::cout << "char map" << std::endl;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			std::cout << charact[i][j];
			if (charact[i][j] != -1)
			{
				Entity* ent = new Entity(entity_type::game_object);

				IDrawable* dc = new IDrawable(ent, IDrawable::layers::surface);
				ent->add_component(dc);
				dc->id  = charact[i][j];
				Transform* transf_c = new Transform(ent);
				ent->add_component(transf_c);
				transf_c->position = {
					static_cast<int>(j*tr->tile_width),
					static_cast<int>(i*tr->tile_width),
					static_cast<int>(tr->tile_width),
					static_cast<int>(tr->tile_width)
				};

				IAnimatable* ac = new IAnimatable(ent);//character sprite size 64x64
				ac->src_rect.w = ac->src_rect.h = static_cast<int>(64);
				ent->add_component(ac);

				character_type type;
				switch (charact[i][j])
				{
				case 1:
					ent->name = "player";
					type = character_type::h_giovanni;
					break;
				default:
					ent->name = "Unnamed";
					type = character_type::zakra_spearman;
					break;
				}
				
				
				ICharacter* cc = new ICharacter(ent, character_type::h_giovanni);
				ent->add_component(cc);

				characters.push_back(ent);
			}
			

		}
		std::cout << std::endl;
	}
	return characters;
}

void character_system::attach_textures_to_characters()
{
	for (unsigned int i = 0; i < characters.size(); i++)
	{
		Entity* charact = characters.at(i);

		IAnimatable* ac = static_cast<IAnimatable*>(charact->get_component(ComponentType::Animated));
		Transform* tc = static_cast<Transform*>(charact->get_component(ComponentType::Transf));
		IDrawable* dc = static_cast<IDrawable*>(charact->get_component(ComponentType::Drawable));
		ICharacter* cc = static_cast<ICharacter*>(charact->get_component(ComponentType::Character));
		
		switch (cc->c_type)
		{
		case character_type::h_giovanni:
			ac->spritesheet = asset_controller::load_texture("assets/graphics/characters/giovanni.png");
			SDL_Rect r = asset_controller::get_texture_size(ac->spritesheet);
			//tc->position.y -= r.h;
			ac->src_rect = {0,0, r.w, r.h};

			dc->sprite = asset_controller::get_sprite_from_spritesheet(ac->spritesheet, ac->src_rect);
			dc->draw_rect = tc->position;

			break;
		}
		
	}
}

character_system::character_system()
{
}


character_system::~character_system()
{
}
