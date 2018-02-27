#include "MainMenuSystem.h"

MainMenuSystem::func_rem MainMenuSystem::dereg_listener;
int MainMenuSystem::mouse_down_listener_id;
int MainMenuSystem::mouse_up_listener_id;

std::vector<void(*)()> MainMenuSystem::exit_listeners;
Entity* MainMenuSystem::mouse;

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
	background->type = entity_type::background;
	Transform* background_transf = new Transform;
	background_transf->position.x = 0;
	background_transf->position.y = 0;
	SDL_manager::get_window_rect(&background_transf->position.w, &background_transf->position.h);
	background_transf->isActive = true;
	background_transf->owner = background;
	background_transf->type = ComponentType::Transf;

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
		Entity* button_ent = new Entity();
		button_ent->type = entity_type::ui_element;
		
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
		button_transf->type = ComponentType::Transf;
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

#pragma region mouse
	Entity* mouse = new Entity();
	mouse->type = entity_type::mouse;
	Transform* m_transform = new Transform();
	m_transform->isActive = true;
	m_transform->position = { 1,1,0,0 };
	m_transform->type = ComponentType::Transf;
	m_transform->owner = mouse;
	m_transform->isActive = true;
	

	IDrawable* m_draw_comp = new IDrawable();
	m_draw_comp->sprite = asset_controller::load_texture("assets/graphics/ui/mouse.png");
	m_draw_comp->type = ComponentType::Drawable;
	m_draw_comp->layer = IDrawable::layers::foreground;
	m_draw_comp->owner = mouse;
	m_draw_comp->isActive = true;

	m_draw_comp->draw_rect = asset_controller::get_texture_size(m_draw_comp->sprite);
	m_draw_comp->draw_rect.x = 0;
	m_draw_comp->draw_rect.y = 0;
	m_transform->position.w = m_draw_comp->draw_rect.w;
	m_transform->position.h = m_draw_comp->draw_rect.h;

	mouse->transform = m_transform;
	mouse->add_component(m_draw_comp);

	MainMenuSystem::mouse = mouse;

	space.objects.push_back(mouse);
#pragma endregion

	add_space_to_render_queue(space);
	space.initialised = true;
}

void MainMenuSystem::update_space(Space & space, int dt)
{
	IDrawable* dc = static_cast<IDrawable*>(mouse->get_component(ComponentType::Drawable));
	mouse->transform->position.x = input_system::mouse.x;
	mouse->transform->position.y = input_system::mouse.y;
	dc->draw_rect.x = input_system::mouse.x;
	dc->draw_rect.y = input_system::mouse.y;

	for (int i = 0; i < space.objects.size(); i++)
	{
		if (space.objects.at(i)->type == entity_type::ui_element)
		{
			Transform* temp = static_cast<Transform*>(find_component_on_object(space.objects.at(i), ComponentType::Transf));
			if (sdl_utils::is_point_in_rect({ mouse->transform->position.x, mouse->transform->position.y }, temp->position))
				std::cout << "mouse within button" << std::endl;
		}
	}
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
