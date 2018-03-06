#include "level_loading_system.h"



void level_loading_system::load_new_game()
{

}

void level_loading_system::init_space(MenuLayout layout, Space & space)
{
	if (space.initialised)
		return;

	Entity* background = new Entity(entity_type::background);
	const char* path = "assets/graphics/bg/loading_bg_1.png";

	Transform* bg_transform = new Transform(background);
	IDrawable* bg_draw = new IDrawable(background, IDrawable::layers::background);
	bg_draw->sprite = asset_controller::load_texture(path);

	SDL_manager::get_window_rect(&bg_transform->position.w, &bg_transform->position.h);
	bg_draw->draw_rect = { 0,0,bg_transform->position.w,bg_transform->position.h };
	//asset_controller::set_texture_alpha(draw->sprite, 0);

	background->add_component(bg_transform);
	background->add_component(bg_draw);
	space.objects.push_back(background);
	space.initialised = true;

	Entity* progress_bar = new Entity(entity_type::ui_element);

	Transform* bar_transform = new Transform(progress_bar);
	IDrawable* bar_draw = new IDrawable(progress_bar, IDrawable::layers::surface);
	

	//level_loading_system::elapsed_stage_time = 0;
	add_space_to_render_queue(space);
}

void level_loading_system::update_space(Space & space, Space & level_space)
{

}



level_loading_system::level_loading_system()
{
}


level_loading_system::~level_loading_system()
{
}
