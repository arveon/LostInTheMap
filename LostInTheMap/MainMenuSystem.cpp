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
#pragma endregion

#pragma region initialise all buttons
	for (int i = 0; i < layout.buttons.size(); i++)
	{
		Button button = layout.buttons.at(i);
		Entity* button_ent;
		Transform* button_transf = new Transform;
		button_transf->isActive = true;
		

		IAnimatable* animation = new IAnimatable;
		
	}
#pragma endregion


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
