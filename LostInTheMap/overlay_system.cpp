#include "overlay_system.h"

Entity * overlay_system::fade;
int overlay_system::total_time = 0;
int overlay_system::elapsed_time;
int overlay_system::wait_time;
overlay_system::fade_state overlay_system::cur_state;
overlay_system::fade_state overlay_system::next_state;
void(*overlay_system::fade_done_callback)(Entity*);

void overlay_system::update(int dt)
{
	if (dt > 20)
		return;
	if (cur_state == waiting)
	{
		elapsed_time += dt;
		if (elapsed_time >= wait_time)
		{
			elapsed_time = 0;
			cur_state = next_state;
		}
		if (next_state == fading_in)
		{
			IDrawable* comp = static_cast<IDrawable*>(fade->get_component(Component::ComponentType::Drawable));
			float cur_alpha = 255;
			if (comp)
				asset_controller::set_texture_alpha(comp->sprite, cur_alpha);
		}
	}
	if (cur_state == fading_in || cur_state == fading_out)
	{
		elapsed_time += dt;

		if (elapsed_time >= total_time)
		{
			elapsed_time = 0;
			if (fade_done_callback)
			{
				fade_done_callback(nullptr);
				fade_done_callback = nullptr;
			}
			cur_state = none;
		}

		IDrawable* comp = static_cast<IDrawable*>(fade->get_component(Component::ComponentType::Drawable));
		if (cur_state == fading_in)
		{
			float cur_alpha = static_cast<int>(255 - (elapsed_time / (float)total_time * 255));
			if (comp)
				asset_controller::set_texture_alpha(comp->sprite, cur_alpha);
		}
		else if (cur_state == fading_out)
		{
			float cur_alpha = static_cast<int>(elapsed_time / (float)total_time * 255);
			if (comp)
				asset_controller::set_texture_alpha(comp->sprite, cur_alpha);
		}

	}
}

overlay_system::overlay_system()
{
}


overlay_system::~overlay_system()
{
}
