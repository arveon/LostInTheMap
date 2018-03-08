#include "level_loading_system.h"

int level_loading_system::t_elapsed_time;
int level_loading_system::loading_progress;
level_loading_system::loading_state level_loading_system::loading_stage;
std::vector<void(*)()> level_loading_system::loading_done_listeners;



void level_loading_system::load_new_game()
{

}

void level_loading_system::init_space(MenuLayout layout, Space & space)
{
	if (space.initialised)
		return;
	//temporary for testing
	t_elapsed_time = 0;
	loading_progress = 0;
	loading_stage = loading_state::loading_misc;

	Entity* background = new Entity(entity_type::background);

	Transform* bg_transform = new Transform(background);
	IDrawable* bg_draw = new IDrawable(background, IDrawable::layers::background);
	bg_draw->sprite = asset_controller::load_texture(layout.background_path.c_str());

	SDL_manager::get_window_rect(&bg_transform->position.w, &bg_transform->position.h);
	bg_draw->draw_rect = { 0,0,bg_transform->position.w,bg_transform->position.h };
	//asset_controller::set_texture_alpha(draw->sprite, 0);

	background->add_component(bg_transform);
	background->add_component(bg_draw);
	space.objects.push_back(background);
	space.initialised = true;

#pragma region initialise all other elements
	for (unsigned int i = 0; i < layout.buttons.size(); i++)
	{
		ui_element_config elm = layout.buttons.at(i);
		Entity* element = new Entity(entity_type::ui_element);

		if (elm.type == UI_Element_Type::loading_bar)
		{
			//bar
			Transform* bar_transf = new Transform(element);
			element->transform = bar_transf;

			IDrawable* drawable = new IDrawable(element, IDrawable::layers::surface);
			drawable->sprite = asset_controller::load_texture("assets/graphics/ui/load_bar.png");
			drawable->draw_rect = asset_controller::get_texture_size(drawable->sprite);
			drawable->draw_rect = { elm.position.x, elm.position.y, drawable->draw_rect.w*2, drawable->draw_rect.h * 2 };
			element->add_component(drawable);

			bar_transf->position = {elm.position.x, elm.position.y, drawable->draw_rect.w, drawable->draw_rect.h};

			Entity* fill = new Entity(entity_type::ui_element);
			fill->name = "fill";
			//bar_fill
			Transform* fill_transf = new Transform(fill);
			fill->transform = fill_transf;

			IAnimatable* fill_anim = new IAnimatable(fill);
			fill_anim->spritesheet = asset_controller::load_texture("assets/graphics/ui/load_fill.png");
			//SDL_Rect temp = asset_controller::get_texture_size(fill_anim->spritesheet);
			fill_anim->src_rect = { 0,0,drawable->draw_rect.w,drawable->draw_rect.h };
			fill->add_component(fill_anim);

			IDrawable* fill_drawable = new IDrawable(fill, IDrawable::layers::foreground);
			fill_drawable->draw_rect = fill_anim->src_rect;
			fill_drawable->sprite = asset_controller::get_sprite_from_spritesheet(fill_anim->spritesheet, fill_anim->src_rect);
			fill_drawable->draw_rect.x = elm.position.x;
			fill_drawable->draw_rect.y = elm.position.y;
			fill->add_component(fill_drawable);

			

			fill_transf->position = { elm.position.x, elm.position.y, fill_drawable->draw_rect.w, fill_drawable->draw_rect.h };
			space.objects.push_back(fill);
		}

		space.objects.push_back(element);
	}
#pragma endregion

	//level_loading_system::elapsed_stage_time = 0;
	add_space_to_render_queue(space);
}

void level_loading_system::update_space(Space & space, Space & level_space, int dt)
{
	t_elapsed_time += dt;
	if (t_elapsed_time >= t_total_time)
	{
		t_elapsed_time = 0;
		loading_stage = loading_state::done;

		loading_done();
	}
	loading_progress = static_cast<int>((float)t_elapsed_time / (float)t_total_time * 100);
	Entity* fill = SpaceSystem::find_entity_by_name(space,"fill");
	if (!fill)
		return;

	IDrawable* draw = static_cast<IDrawable*>(fill->get_component(ComponentType::Drawable));
	IAnimatable* anim = static_cast<IAnimatable*>(fill->get_component(ComponentType::Animated));
	Transform* tr = static_cast<Transform*>(fill->get_component(ComponentType::Transf));
	//get whole spritesheet width
	SDL_Rect ss_size = asset_controller::get_texture_size(anim->spritesheet);

	//calculate desired width
	int d_w = tr->position.w * loading_progress / 100;

	//make src_rect.w equal to that width
	anim->src_rect.w = d_w/2;

	//make draw_rect.w equal to that width
	draw->draw_rect.w = d_w;

	//remove old sprite
	asset_controller::destroy_texture(draw->sprite);

	//update sprite to new sprite
	draw->sprite = asset_controller::get_sprite_from_spritesheet(anim->spritesheet, anim->src_rect);
}

void level_loading_system::loading_done()
{
	std::cout << "loading done" << std::endl;
	for (unsigned int i = 0; i < loading_done_listeners.size(); i++)
	{
		void(*listener)() = loading_done_listeners.at(i);
		listener();
	}
}


level_loading_system::level_loading_system()
{
}


level_loading_system::~level_loading_system()
{
}
