#include "MainMenuSystem.h"

MainMenuSystem::func_rem MainMenuSystem::dereg_listener;
int MainMenuSystem::mouse_down_listener_id;
int MainMenuSystem::mouse_up_listener_id;

std::vector<void(*)()> MainMenuSystem::exit_listeners;

void MainMenuSystem::init(Space & space, MenuLayout layout, func_reg lmb_down_reg, func_reg lmb_up_reg, func_rem deregister_callback)
{
	dereg_listener = deregister_callback;
	mouse_down_listener_id = lmb_down_reg(&mouse_down_listener);
	mouse_up_listener_id = lmb_up_reg(&mouse_up_listener);


	init_space(space, layout);
}

void MainMenuSystem::init_space(Space & space, MenuLayout layout)
{
	//initialising background
#pragma region background
	Entity* background = new Entity();
	Transform* background_transf = new Transform;
	background_transf->position.x = 0;
	background_transf->position.y = 0;
	SDL_manager::get_window_rect(&background_transf->position.w, &background_transf->position.h);
	background_transf->isActive = true;
	background_transf->owner = background;
	background_transf->type = ComponentType::Location;

	IDrawable* background_draw_component = new IDrawable;
	background_draw_component->sprite = asset_controller::load_texture(layout.background_path.c_str());
	background_draw_component->draw_rect = background_transf->position;
	background_draw_component->isActive = true;
	background_draw_component->owner = background;
	background_draw_component->type = ComponentType::Drawable;

	background->transform = background_transf;
	background->add_component(background_draw_component);
	space.objects.push_back(background);
#pragma endregion

#pragma region initialise all buttons
	for (unsigned int i = 0; i < layout.buttons.size(); i++)
	{
		Button button = layout.buttons.at(i);
		Entity* button_ent = new Entity();;
		
		//create animation component
		IAnimatable* animation = new IAnimatable;
		animation->spritesheet = asset_controller::create_ui_text_button_spritesheet(button.name, UI_text_type::main_menu_button_main);
		SDL_Rect temp = asset_controller::get_texture_size(animation->spritesheet);
		animation->src_rect.w = temp.w / 4;
		animation->src_rect.h = temp.h;
		animation->src_rect.x = 0;
		animation->src_rect.y = 0;
		animation->owner = button_ent;
		animation->type = ComponentType::Animated;
		button_ent->add_component(animation);

		Transform* button_transf = new Transform;
		button_transf->isActive = true;
		button_transf->position = { button.position.x, button.position.y, animation->src_rect.w, animation->src_rect.h };
		button_transf->owner = button_ent;
		button_transf->type = ComponentType::Location;
		button_ent->transform = button_transf;

		IDrawable* drawable = new IDrawable;
		drawable->sprite = asset_controller::get_sprite_from_spritesheet(animation->spritesheet, animation->src_rect);
		drawable->draw_rect = button_transf->position;
		drawable->owner = button_ent;
		drawable->type = ComponentType::Drawable;
		button_ent->add_component(drawable);
		
		space.objects.push_back(button_ent);
	}
#pragma endregion

	add_space_to_render_queue(space);
	space.initialised = true;
}

void MainMenuSystem::update_space(Space & space, int dt)
{





}

void MainMenuSystem::destroy_space(Space & space)
{
	space.objects.clear();
	space.initialised = false;

	dereg_listener(HardInputEventType::left_mouse_down, mouse_down_listener_id);
	dereg_listener(HardInputEventType::left_mouse_up, mouse_up_listener_id);
}

MainMenuSystem::MainMenuSystem()
{
}


MainMenuSystem::~MainMenuSystem()
{
}
