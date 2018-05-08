#include "animator.h"

std::map<IAnimatable*, void(*)(Entity* source)> animator::animation_done_callbacks;

void animator::update(Space& game_space, int dt)
{
	if (dt > 20)
		dt = 1;
	for (Entity* e : game_space.objects)
	{
		IAnimatable* ac = static_cast<IAnimatable*>(e->get_component(Component::ComponentType::Animated));
		if (!ac)
			continue;
		if (!ac->animation_started || ac->animation_finished)
			continue;

		ac->time_elapsed += dt;
		if (ac->time_elapsed >= ac->total_sprite_required_time)
		{
			ac->time_elapsed = 0;
			SDL_Rect size = asset_controller::get_texture_size(ac->spritesheet);
			ac->cur_column++;
			if (ac->cur_column >= ac->num_frames)
				ac->cur_column = 0;
			//ac->cur_column = (ac->cur_column + 1 >= max_frames_in_animation) ? 0 : ac->cur_column+1;
			
			if (ac->cur_column == 0 && animation_done_callbacks[ac] != nullptr)
			{
				ac->cur_column = max_frames_in_animation - 1;
				ac->animation_finished = true;
				void(*a)(Entity*) = animation_done_callbacks[ac];
				animation_done_callbacks[ac] = nullptr;
				a(ac->owner);
			}
			ac->sprite_changed = true;
		}
	}

}

void animator::start_animation(IAnimatable* ac, animations type, void(*done_callback)(Entity* source))
{
	bool already_there = false;
	switch (type)
	{
	case animations::idle:
	{
		IDrawable * dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		ICombatUnit* cbu = (ICombatUnit*)ac->owner->get_component(Component::ComponentType::CombatUnit);
		if (cbu)
			if (cbu->unit_stats->is_enemy)
				dc->flipped_x = true;
			else
				dc->flipped_x = false;
		else
			dc->flipped_x = false;
		if (ac->cur_row == 0)
			already_there = true;
		ac->cur_row = 0;
		ac->total_sprite_required_time = 300;
	}
		break;
	case animations::walking_right:
	{		
		ac->cur_row = 1;
		IDrawable* dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		if (dc)
		{
			if (ac->cur_row == 1 && !dc->flipped_x)
				already_there = true;
			dc->flipped_x = false;
		}
		ac->total_sprite_required_time = 100;
	}
		break;
	case animations::walking_left:
	{
		ac->cur_row = 1;
		IDrawable* dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		if (dc)
		{
			if (ac->cur_row == 1 && dc->flipped_x)
				already_there = true;
			dc->flipped_x = true;
		}
		ac->total_sprite_required_time = 100;
	}
		break;
	case animations::object_active:
	case animations::walking_up:
		if (ac->cur_row == 2)
			already_there = true;
		ac->total_sprite_required_time = 100;
		ac->cur_row = 2;
		break;
	case animations::walking_down:
		if (ac->cur_row == 3)
			already_there = true;
		ac->total_sprite_required_time = 100;
		ac->cur_row = 3;
		break;
	case animations::attacking_from_left:
	{
		IDrawable * dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		if (dc)
		{
			if (ac->cur_row == 4 && !dc->flipped_x)
				already_there = true;
			dc->flipped_x = false;
		}
		ac->total_sprite_required_time = 100;
		ac->cur_row = 4;
	}
		break;
	case animations::attacking_from_right:
	{
		IDrawable * dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		if (dc)
		{
			if (ac->cur_row == 4 && dc->flipped_x)
				already_there = true;
			dc->flipped_x = true;
		}
		ac->total_sprite_required_time = 100;
		ac->cur_row = 4;
	}
		break;
	case animations::taking_damage:
		if (ac->cur_row == 5)
			already_there = true;
		ac->cur_row = 5;
		ac->total_sprite_required_time = 100;
		break;
	case animations::death:
		if (ac->cur_row == 6)
			already_there = true;
		ac->cur_row = 6;
		ac->total_sprite_required_time = 100;
		break;
	case animations::selected:
		if (ac->cur_row == 7)
			already_there = true;
		ac->cur_row = 7;
		ac->total_sprite_required_time = 170;
		break;
	case animations::object_animation:
		if (ac->cur_row == 1)
			already_there = true;
		ac->cur_row = 1;
		ac->total_sprite_required_time = 300;
		break;
	}
	if (!already_there)
	{
		ac->cur_column = 0;
		ac->sprite_changed = true;
		ac->animation_finished = false;
		ac->animation_started = true;
		ac->time_elapsed = 0;
	}
	if (done_callback != nullptr)
		animation_done_callbacks[ac] = done_callback;
}

void animator::apply_animation_sprite_changes(Space& space)
{
	for (Entity* e : space.objects)
	{
		IAnimatable* ac = static_cast<IAnimatable*>(e->get_component(Component::ComponentType::Animated));
		if (!ac || !ac->sprite_changed)
			continue;

		IDrawable* dc = static_cast<IDrawable*>(e->get_component(Component::ComponentType::Drawable));
		if (!dc)
			continue;

		asset_controller::destroy_texture(dc->sprite);
		//apply cur coordinate changes to src rect and crop src rect from spritesheet into draw components sprite
		ac->src_rect.x = ac->cur_column * ac->src_rect.w;
		ac->src_rect.y = ac->cur_row* ac->src_rect.h;
		dc->sprite = asset_controller::get_sprite_from_spritesheet(ac->spritesheet, ac->src_rect);
		ac->sprite_changed = false;

	}
}

void animator::set_walking_animation(Entity* ent, ITerrain* tc)
{
	IAnimatable* ac = (IAnimatable*)ent->get_component(Component::ComponentType::Animated);
	IMoving* mc = (IMoving*)ent->get_component(Component::ComponentType::Movement);

	/*if (mc->path.size() > 0)
	{
		SDL_Point next_dest = *mc->path.begin();
		SDL_Point pos = map_system::get_entity_ids(ent, tc);


		if (next_dest.x > pos.x)
			animator::start_animation(ac, animations::walking_right);
		else if (next_dest.x < pos.x)
			animator::start_animation(ac, animations::walking_left);
		else
		{
			if (next_dest.y > pos.y)
				animator::start_animation(ac, animations::walking_down);
			else if (next_dest.y < pos.y)
				animator::start_animation(ac, animations::walking_up);
		}
	}*/
	if (mc->shift_buffer_x > 0)
		animator::start_animation(ac, animations::walking_right);
	else if (mc->shift_buffer_x < 0)
		animator::start_animation(ac, animations::walking_left);
	/*else if (mc->shift_buffer_y < 0)
		animator::start_animation(ac, animations::walking_left);*/
	
}


animator::animator()
{
}

animator::~animator()
{
}
