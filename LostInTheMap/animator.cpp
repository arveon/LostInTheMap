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
			ac->cur_column = (ac->cur_column + 1 >= max_frames_in_animation) ? 0 : ac->cur_column+1;
			if (ac->cur_column == 0 && animation_done_callbacks[ac] != nullptr)
			{
				animation_done_callbacks[ac](ac->owner);
				animation_done_callbacks[ac] = nullptr;
				ac->cur_column = max_frames_in_animation-1;
				ac->animation_finished = true;
			}
			ac->sprite_changed = true;
		}



	}

}

void animator::start_animation(IAnimatable* ac, animations type, void(*done_callback)(Entity* source))
{
	switch (type)
	{
	case animations::idle:
		ac->cur_row = 0;
		break;
	case animations::walking_right:
	{		
		ac->cur_row = 1;
		IDrawable* dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		if (dc)
		dc->flipped_x = false;
	}
		break;
	case animations::walking_left:
	{
		ac->cur_row = 1;
		IDrawable* dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		if (dc)
			dc->flipped_x = true;
	}
		break;
	case animations::walking_up:
		ac->cur_row = 2;
	case animations::walking_down:
		ac->cur_row = 3;
		break;
	case animations::attacking_right:
	{
		IDrawable * dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		if (dc)
			dc->flipped_x = false;
		ac->cur_row = 4;
	}
		break;
	case animations::attacking_left:
	{
		IDrawable * dc = (IDrawable*)ac->owner->get_component(Component::ComponentType::Drawable);
		if (dc)
			dc->flipped_x = true;
		ac->cur_row = 4;
	}
		break;
	case animations::taking_damage:
		ac->cur_row = 5;
		break;
	case animations::death:
		ac->cur_row = 6;
		break;
	}
	ac->cur_column = 0;
	ac->sprite_changed = true;
	ac->animation_finished = false;
	ac->animation_started = true;
	ac->time_elapsed = 0;

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


animator::animator()
{
}

animator::~animator()
{
}
