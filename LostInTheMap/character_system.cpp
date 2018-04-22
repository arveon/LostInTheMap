#include "character_system.h"

std::vector<Entity*> character_system::characters;

std::vector<Entity*> character_system::init_characters(Actor** charact, int width, int height, ITerrain* tr, bool create_armies)
{
	static int value = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//if there is a character in this tile initialise an object for it
			//TODO: UNCOMMENT CHARACTER LOADING WHEN ALL LEVEL TRANSITIONS READY
			if (charact[i][j].value == 2 /*|| charact[i][j].value == 3*/ || charact[i][j].value == 1)
			{
				Entity* ent = new Entity(entity_type::game_object);

				//create all required components for the character
				IDrawable* dc = new IDrawable(ent, IDrawable::layers::surface);
				ent->add_component(dc);
				dc->id = charact[i][j].value;
				Transform* transf_c = new Transform(ent);
				ent->add_component(transf_c);
				transf_c->position = {
					static_cast<int>(j*tr->tile_width),
					static_cast<int>(i*tr->tile_width),
					static_cast<int>(tr->tile_width),
					static_cast<int>(tr->tile_width)
				};
				transf_c->origin = {transf_c->position.w/2, transf_c->position.h-1};

				ICollidable* colc = new ICollidable(ent);
				colc->collidable = true;
				colc->collision_rect = { 0,0,tr->tile_width,tr->tile_width };
				ent->add_component(colc);

				IAnimatable* ac = new IAnimatable(ent);//character sprite size 64x64
				ac->src_rect.w = ac->src_rect.h = static_cast<int>(64);
				ent->add_component(ac);

				IMoving* mc = new IMoving(ent, j, i);
				mc->final_destination = { -1,-1 };
				ent->add_component(mc);

				//set name and character type depending on what's stored in field
				character_type type = xml_system::get_character_type_by_name(charact[i][j].type);
				bool is_friendly = true;
				switch (type)
				{
				case character_type::h_giovanni:
					ent->name = "player";
					mc->pathfinder.is_player = true;
					break;
				case character_type::npc_arch_supervisor:
					ent->name = "supervisor" + std::to_string(value);
					value++;
					break;
				case character_type::npc_archaeologist_1:
				case character_type::npc_archaeologist_2:
				case character_type::npc_archaeologist_3:
				case character_type::npc_archaeologist_4:
					ent->name = "archaeologist" + std::to_string(value);
					value++;
					break;
				case character_type::rat:
					ent->name = "rat" + std::to_string(value);
					value++;
					is_friendly = false;
					break;
				case character_type::h_jido:
					ent->name = "jido" + std::to_string(value);
					value++;
					break;
				case character_type::h_josi:
					ent->name = "josi" + std::to_string(value);
					value++;
					break;
				case character_type::juji_villager_1:
				case character_type::juji_villager_2:
				case character_type::juji_villager_3:
				case character_type::juji_villager_4:
					ent->name = "villager" + std::to_string(value);
					value++;
					break;
				case character_type::h_zaji:
					ent->name = "zaji" + std::to_string(value);
					value++;
					break;
				case character_type::snake:
					ent->name = "snake" + std::to_string(value);
					value++;
					is_friendly = false;
					break;
				case character_type::zakra_spearman:
					ent->name = "spearman" + std::to_string(value);
					value++;
					is_friendly = false;
					break;
				
				default:
					ent->name = "Unnamed";
					type = character_type::zakra_spearman;
					break;
				}

				//if its an enemy, it should be an interaction source
				IInteractionSource* src = new IInteractionSource(ent);
				src->interaction_trigger = &director::process_interaction;
				src->script_attached = charact[i][j].script;
					

				ent->add_component(src);

				if ((!is_friendly || type==character_type::h_giovanni) && create_armies)
				{
					IFightable* fc = new IFightable(ent);
					fc->army_file = charact[i][j].army;
					ent->add_component(fc);

					if (src->script_attached.compare("") == 0)
						src->script_attached = "start_combat.xml";
					
					character_system::add_army_to_character(ent);
				}

				ICharacter* cc = new ICharacter(ent, type, is_friendly);
				ent->add_component(cc);

				if (type == character_type::h_giovanni)
				{
					IInteractionSource* src = new IInteractionSource(ent);
					src->interaction_trigger = &director::process_interaction;
					ent->add_component(src);
				}

				characters.push_back(ent);
			}
		}
	}
	return characters;
}

void character_system::attach_textures_to_characters(SDL_Point tile_origin)
{
	for (unsigned int i = 0; i < characters.size(); i++)
	{
		Entity* charact = characters.at(i);

		IAnimatable* ac = static_cast<IAnimatable*>(charact->get_component(Component::ComponentType::Animated));
		Transform* tc = static_cast<Transform*>(charact->get_component(Component::ComponentType::Transf));
		IDrawable* dc = static_cast<IDrawable*>(charact->get_component(Component::ComponentType::Drawable));
		ICharacter* cc = static_cast<ICharacter*>(charact->get_component(Component::ComponentType::Character));
		ICollidable* colc = static_cast<ICollidable*>(charact->get_component(Component::ComponentType::Collision));

		dc->draw_rect = tc->position;

		//init texture and draw rect size
		std::string tex_path = "assets/graphics/characters/"+xml_system::get_character_name_by_type(cc->c_type)+".png";
		ac->spritesheet = asset_controller::load_texture(tex_path.c_str());
		dc->draw_rect.w = 50;
		dc->draw_rect.h = 48;
		if (cc->c_type == character_type::zakra_spearman)
		{
			dc->draw_rect.w = 64;
			dc->draw_rect.h = 64;
		}

		//set tile origin
		dc->sprite_origin = { dc->draw_rect.w / 2, dc->draw_rect.h };		
		
		//set up collision rect
		colc->collision_rect.w = 25;
		colc->collision_rect.h = 10;
		colc->collision_rect.x = tc->origin.x - colc->collision_rect.w / 2;
		colc->collision_rect.y = tc->origin.y - colc->collision_rect.h;

		//set sprite
		SDL_Rect r = asset_controller::get_texture_size(ac->spritesheet);
		ac->src_rect = { 0,0, r.w, r.h };
		dc->sprite = asset_controller::get_sprite_from_spritesheet(ac->spritesheet, ac->src_rect);
	}
}

void character_system::set_final_destination(ITerrain* terrain, Entity* character, SDL_Point mouse_pos, Space& space)
{
	IMoving* mc = static_cast<IMoving*>(character->get_component(Component::ComponentType::Movement));
	ICollidable* colc = static_cast<ICollidable*>(character->get_component(Component::ComponentType::Collision));
	Transform* tra = static_cast<Transform*>(character->get_component(Component::ComponentType::Transf));

	//set pathfinder dest to tile_id
	Entity* tile = map_system::get_tile_at(terrain->owner, mouse_pos);
	ITile* tile_c = static_cast<ITile*>(tile->get_component(Component::ComponentType::Tile));
	Transform* tile_t = static_cast<Transform*>(tile->get_component(Component::ComponentType::Transf));

	SDL_Point char_ids = character_system::get_character_ids(character, terrain);
	mc->pathfinder.set_origin(char_ids);
	//set pathfinders destination to tile
	mc->pathfinder.set_destination({ tile_c->x, tile_c->y });
	//calculate and get path from pathfinder
	mc->path = mc->pathfinder.get_path_to({ tile_c->x, tile_c->y }, true);

	//set destination
	SDL_Point dest_ids = map_system::world_to_tilemap_ids(mouse_pos, terrain);
	Entity* target = SpaceSystem::get_object_at_point(space, mouse_pos.x, mouse_pos.y, true);
	if (target)
	{
		ITile* tc = static_cast<ITile*>(target->get_component(Component::ComponentType::Tile));
		if (tc)
		{//if tile was returned
			Entity* temp = SpaceSystem::get_object_at_ids(space, dest_ids.x, dest_ids.y);
			target = (temp==nullptr || character->name.compare(temp->name)==0) ? target : temp;
			
		}
	}

	//get players interaction source
	IInteractionSource* is = static_cast<IInteractionSource*>(character->get_component(Component::ComponentType::InteractionSource));
	if (is)//make sure player has one on him to avoid crashes
	{
		if (target->is_active)
		{
			//if target is also an interaction source (can be interacted with, store it as players interaction target)
			if (static_cast<IInteractionSource*>(target->get_component(Component::ComponentType::InteractionSource)))
			{
				is->interaction_target = target;
				is->has_triggered = false;
			}
			else
				is->interaction_target = nullptr;
		}
		else
			is->interaction_target = nullptr;
	}

	bool need_to_move = false;
	if (!mc->path.empty())
	{//if moving between tiles
		//set destination mouse click position
		SDL_Point character_origin = character->get_object_origin();

		need_to_move = true;

		//check if point overlaps with any object and this object is not tile and collidable
		ITile* t_test = nullptr;
		ICollidable* c_test = nullptr;
		if (target && target->is_active)
			c_test = static_cast<ICollidable*>(target->get_component(Component::ComponentType::Collision));
		if (c_test && target)
		{
			mc->path.erase(mc->path.begin());

			if (mc->path.empty())
			{
				need_to_move = false;
				goto adding_interaction;
			}
			//get last tile of path
			SDL_Point temp_ids = *(mc->path.begin());
			Entity* temp_tile = terrain->terrain_tiles[temp_ids.y][temp_ids.x];
			ITile* temp_c = static_cast<ITile*>(temp_tile->get_component(Component::ComponentType::Tile));
			Transform* temp_t = static_cast<Transform*>(temp_tile->get_component(Component::ComponentType::Transf));

			mc->final_destination = { temp_c->x * terrain->tile_width + temp_t->origin.x - tra->origin.x, temp_c->y * terrain->tile_width + temp_t->origin.y - tra->origin.y };
		}
		else
		{
			mc->final_destination = { mouse_pos.x - character_origin.x, mouse_pos.y - character_origin.y };
		}

		SDL_Point desired_point = collision_system::resolve_collisions(colc, mc, terrain);
		mc->final_destination.x += desired_point.x;
		mc->final_destination.y += desired_point.y;
		mc->destination_reached = false;
	}
	else if (tile == terrain->terrain_tiles[char_ids.y][char_ids.x] && tile_c->is_traversible)
	{//if moving within the same tile
		//set destination mouse click position
		need_to_move = true;
		SDL_Point player_sp_or = character->get_object_origin();
		mc->final_destination = { mouse_pos.x - player_sp_or.x, mouse_pos.y - player_sp_or.y };
		SDL_Point desired_point = collision_system::resolve_collisions(colc, mc, terrain);
		mc->final_destination.x += desired_point.x;
		mc->final_destination.y += desired_point.y;
		mc->destination_reached = false;
	}

adding_interaction:
	if (!need_to_move)
	{//if don't need to move to destination, trigger the script on players interaction target
		mc->final_destination = { -1,-1 };
		mc->path.clear();
		mc->destination_reached = true;
		if (target && is && is->interaction_target != nullptr)
		{
			//is->has_triggered = true;
			is->interaction_trigger(is->interaction_target);
		}
	}

}

void character_system::set_final_destination_ids(ITerrain * terrain, Entity * character, SDL_Point dest_ids, Space & space)
{
	SDL_Point coords = {0,0};
	coords = map_system::tilemap_ids_to_world(dest_ids, terrain);
	Transform* tf = character->transform;
	coords.x += tf->origin.x;
	coords.y += tf->origin.y;

	set_final_destination(terrain, character, coords, space);
}

SDL_Point character_system::get_character_ids(Entity* character, ITerrain* tc)
{
	SDL_Point player_ids = map_system::world_to_tilemap_ids(character->get_origin_in_world(), tc);
	return player_ids;
}

void character_system::stop_character_movement(Entity * character)
{
	IMoving* mc = static_cast<IMoving*>(character->get_component(Component::ComponentType::Movement));
	if (mc)
	{
		mc->movement_allowed = false;
	}
}

void character_system::allow_character_movement(Entity * character)
{
	IMoving* mc = static_cast<IMoving*>(character->get_component(Component::ComponentType::Movement));
	if (mc)
		mc->movement_allowed = true;
}

void character_system::add_army_to_character(Entity * character)
{
	IFightable* fc = static_cast<IFightable*>(character->get_component(Component::ComponentType::Fighting));
	std::cout << "load army" << std::endl;
	
}

Entity* character_system::get_character(character_type character)
{
	if (character == character_type::none)
		return nullptr;
	for (Entity* ch : characters)
	{
		ICharacter* cc = static_cast<ICharacter*>(ch->get_component(Component::ComponentType::Character));
		if (cc->c_type == character)
			return ch;
	}
	return nullptr;
}

character_system::character_system()
{
}


character_system::~character_system()
{
}
