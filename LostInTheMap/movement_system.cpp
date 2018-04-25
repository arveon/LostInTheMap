#include "movement_system.h"

float movement_system::tps = .1f;

void movement_system::move_characters_tick(Space& game_space, int dt, ITerrain* tr)
{
	//std::cout << "dt = " << dt << std::endl;
	if (dt > 20)
		return;
	int tilewidth = tr->tile_width;
	int tilehigh = tr->tile_height;
	for (unsigned int i = 0; i < game_space.objects.size(); i++)
	{
		Entity* character = game_space.objects.at(i);

		//if character inactive, skip it
		if (!character->is_active)
			continue;

		//get moving component to see if entity is movable
		IMoving* mc = static_cast<IMoving*>(character->get_component(Component::ComponentType::Movement));

		//if not moving, skip
		if (!mc)
			continue;
		if (!mc->movement_allowed)
		{
			//mc->path.clear();
			//mc->destination_reached = true;
			continue;
		}

		Transform* tc = static_cast<Transform*>(character->get_component(Component::ComponentType::Transf));
		IDrawable* dc = static_cast<IDrawable*>(character->get_component(Component::ComponentType::Drawable));

		if (mc->destination_reached && mc->path.size() == 0)//if destination tile reached (or not moving at all)
			continue;

		SDL_Point cur_dest;
		//if next tile is not last destination
		if (mc->path.size() > 1)
		{
			cur_dest = mc->path.back();
			Transform* tile_t = static_cast<Transform*>(tr->terrain_tiles[cur_dest.y][cur_dest.x]->get_component(Component::ComponentType::Transf));
			cur_dest.x = cur_dest.x * tilewidth + tile_t->origin.x - tc->origin.x;
			cur_dest.y = cur_dest.y * tilehigh + tile_t->origin.y - tc->origin.y;

			if (tc->position.x == cur_dest.x && tc->position.y == cur_dest.y)
			{
				SDL_Point ori = mc->path.back();
				mc->pathfinder.set_origin({ ori.x, ori.y });
				mc->path.pop_back();
				mc->path = mc->pathfinder.get_path(false);
				if (mc->path.size() == 0)
					mc->destination_reached = true;

				//only player can cause triggers
				if (character->name.compare("player")==0)
				{
					//check if currently inside trigger
					Entity* trigger_tile = SpaceSystem::get_object_at_point(game_space, tc->position.x + tc->origin.x, tc->position.y + tc->origin.y, false);
					if (trigger_tile && trigger_tile->is_active)
					{
						IInteractionSource* trigger = static_cast<IInteractionSource*>(trigger_tile->get_component(Component::ComponentType::InteractionSource));
						if (trigger)
						{
							if (!trigger->has_triggered)
							{
								trigger->interaction_trigger(trigger_tile);
							}
						}
					}
				}
			}
		}
		else
		{//if one step between tiles remaining
			if (!mc->path.empty())
			{
				SDL_Point ori = mc->path.back();
				mc->pathfinder.set_origin({ ori.x, ori.y });
				mc->path.pop_back();
				mc->path = mc->pathfinder.get_path(false);
			}
			
			//check for collisions and resolve them in cur_dest
			cur_dest.x = mc->final_destination.x;
			cur_dest.y = mc->final_destination.y;

			//if destination reached
			SDL_Point char_origin = character->get_origin_in_world();
			if (tc->position.x == mc->final_destination.x && tc->position.y == mc->final_destination.y)
			{
				tc->position.x = mc->final_destination.x;
				tc->position.y = mc->final_destination.y;
				mc->destination_reached = true;

				//if was controlled by script, call a callback
				ICharacter* ch = static_cast<ICharacter*>(character->get_component(Component::ComponentType::Character));
				if (ch)
				{
					if(ch->controlled_by_script)
						ch->script_done_callback(character);
				}

				//only player can trigger scripts, so don't check for triggers on other characters
				if (character->name.compare("player")==0)
				{
					//get trigger component and check interaction target
					IInteractionSource* int_src = static_cast<IInteractionSource*>(character->get_component(Component::ComponentType::InteractionSource));
					if (int_src)
					{
						if (!int_src->has_triggered && int_src->interaction_target)
						{
							void(*a)(Entity*) = int_src->interaction_trigger;
							a(int_src->interaction_target);
							//int_src->has_triggered = true;
						}
					}

					//check if currently inside trigger
					Entity* trigger_tile = SpaceSystem::get_object_at_point(game_space, tc->position.x + tc->origin.x, tc->position.y + tc->origin.y, false);
					if (trigger_tile && trigger_tile->is_active)
					{
						IInteractionSource* trigger = static_cast<IInteractionSource*>(trigger_tile->get_component(Component::ComponentType::InteractionSource));
						if (trigger)
						{
							if (!trigger->has_triggered)
							{
								trigger->interaction_trigger(trigger_tile);
								//trigger->has_triggered = true;
							}
						}
					}
				}
			}
		}
		movement_system::move_character_transform(dt, cur_dest, tc, mc);
	}
}

void movement_system::move_character_transform(int dt, SDL_Point cur_dest, Transform* tc, IMoving* mc)
{
	int delta_x, delta_y;
	delta_x = cur_dest.x - tc->position.x;
	delta_y = cur_dest.y - tc->position.y;

	double angle = (double)std::atan2(delta_y, delta_x);
	float a = (float)std::cos(angle);
	float b = (float)std::sin(angle);
	float shift_x = a * movement_system::tps*dt;
	float shift_y = b * movement_system::tps*dt;

	//required so lower speeds could be used (buffer accumulates until reached >1
	//if buffer reached whole numbers, it updates the character coordinates and is reduced down to 0.something)
	//static float shift_buffer_x = 0, shift_buffer_y = 0;
	mc->shift_buffer_x += shift_x;
	mc->shift_buffer_y += shift_y;
	if (mc->shift_buffer_x > 1.f || mc->shift_buffer_x < -1.f)
	{
		//since floor rounds negative values down towards negative (-0.5 to -1) and ceil rounds them up towards positive (-0.5 to 0)
		//without this if statement speed is larger towards negative 
		if (mc->shift_buffer_x > 0)
			tc->position.x += (int)std::floor(mc->shift_buffer_x);
		else
			tc->position.x += (int)std::ceil(mc->shift_buffer_x);
		//reset shift buffer
		if (mc->shift_buffer_x > 0)
			mc->shift_buffer_x -= 1.f;
		else
			//tc->position.x -= std::ceil(shift_buffer_x);
			mc->shift_buffer_x += 1.f;
	}

	if (mc->shift_buffer_y > 1.f || mc->shift_buffer_y < -1.f)
	{
		if (mc->shift_buffer_y > 0)
			tc->position.y += (int)std::floor(mc->shift_buffer_y);
		else
			tc->position.y += (int)std::ceil(mc->shift_buffer_y);
		//reset shift buffer
		if (mc->shift_buffer_y > 0)
			mc->shift_buffer_y -= 1.f;
		else
			mc->shift_buffer_y += 1.f;
	}
}

movement_system::movement_system()
{
}


movement_system::~movement_system()
{
}
