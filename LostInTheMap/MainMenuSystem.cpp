#include "MainMenuSystem.h"

MainMenuSystem::func_rem MainMenuSystem::dereg_listener;
int MainMenuSystem::mouse_down_listener_id;
int MainMenuSystem::mouse_up_listener_id;

std::vector<void(*)()> MainMenuSystem::exit_listeners;
std::vector<void(*)()> MainMenuSystem::start_listeners;
std::vector<void(*)()> MainMenuSystem::load_listeners;
std::vector<void(*)()> MainMenuSystem::settings_listeners;
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
	camera_system::set_camera_zoom(1.f);
	//initialising background
#pragma region background
	Entity* background = new Entity(entity_type::background);
	Transform* background_transf = new Transform(background);
	SDL_manager::get_window_size(&background_transf->position.w, &background_transf->position.h);

	IDrawable* background_draw_component = new IDrawable(background, IDrawable::layers::background);
	background_draw_component->sprite = asset_controller::load_texture(layout.background_path.c_str());
	background_draw_component->draw_rect = background_transf->position;

	background->transform = background_transf;
	background->add_component(background_draw_component);
	space.objects.push_back(background);
#pragma endregion
	
#pragma region initialise all buttons
	for (unsigned int i = 0; i < layout.buttons.size(); i++)
	{
		ui_element_config button = layout.buttons.at(i);
		Entity* element = new Entity(entity_type::ui_element);

		if (button.type == UI_Element_Type::button)
		{
			//create animation component
			IAnimatable* animation = new IAnimatable(element);
			animation->spritesheet = asset_controller::create_ui_text_button_spritesheet(button.name, UI_text_type::main_menu_button_main);
			SDL_Rect temp = asset_controller::get_texture_size(animation->spritesheet);
			animation->src_rect = { 0,0, temp.w / 5, temp.h };
			element->add_component(animation);

			Transform* button_transf = new Transform(element);
			button_transf->position = { button.position.x, button.position.y, animation->src_rect.w, animation->src_rect.h };
			element->transform = button_transf;

			IDrawable* drawable = new IDrawable(element, IDrawable::layers::surface);
			drawable->sprite = asset_controller::get_sprite_from_spritesheet(animation->spritesheet, animation->src_rect);
			drawable->draw_rect = button_transf->position;
			element->add_component(drawable);

			IUIElement* uic = new IUIElement(element);
			uic->element_type = UI_Element_Type::button;
			uic->name = button.name;
			element->add_component(uic);


			space.objects.push_back(element);
		}
	}
#pragma endregion

#pragma region mouse
	MainMenuSystem::mouse = mouse_system::create_mouse();
	space.objects.push_back(mouse);
#pragma endregion

	add_space_to_render_queue(space);
	
	space.initialised = true;
}

void MainMenuSystem::update_space(Space & space, int dt)
{
#pragma region update_mouse
	mouse_system::update_mouse(MainMenuSystem::mouse, space, true, false);
#pragma endregion

	//check mouse collisions with other objects
	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		Entity* temp_e = space.objects.at(i);
		if (temp_e->type == entity_type::ui_element)
		{		
			Transform* temp_t = static_cast<Transform*>(temp_e->get_component(Component::ComponentType::Transf));
			IMouse* mc = static_cast<IMouse*>(mouse->get_component(Component::ComponentType::Mouse));
			//IAnimatable* anim = static_cast<IAnimatable*>(find_component_on_object(space.objects.at(i), ComponentType::Animated));
			if (sdl_utils::is_point_in_rect({ mouse->transform->position.x, mouse->transform->position.y }, temp_t->position))
			{
				if (mc->cur_target != temp_e)
				{
					mouse_target_changed(temp_e);
				}
			}
			else if (mc->cur_target == temp_e)
				mouse_target_changed(nullptr);
			
		}
	}
	//std::cout << dt << std::endl;
	SpaceSystem::apply_animation_sprite_changes(space);
}

void MainMenuSystem::destroy_space(Space & space)
{
	dereg_listener(HardInputEventType::left_mouse_down, mouse_down_listener_id);
	dereg_listener(HardInputEventType::left_mouse_up, mouse_up_listener_id);
	
	SpaceSystem::destroy_space(space);
}

void MainMenuSystem::mouse_target_changed(Entity* new_target)
{
	IMouse* mouse = static_cast<IMouse*>(MainMenuSystem::mouse->get_component(Component::ComponentType::Mouse));
	
	//if mouse had a target before, update it and remove the reference to it from mouse
	if (mouse->cur_target != nullptr)
	{
		IAnimatable* ac_old = static_cast<IAnimatable*>(mouse->cur_target->get_component(Component::ComponentType::Animated));
		if (ac_old->src_rect.x != 0)
		{
			ac_old->src_rect.x = 0;
			ac_old->sprite_changed = true;
			mouse->cur_target = nullptr;
		}
	}
	
	//update new target and add reference to it to mouse
	if (new_target != nullptr)
	{
		IAnimatable* ac_new = static_cast<IAnimatable*>(new_target->get_component(Component::ComponentType::Animated));
		if (new_target != nullptr)
		{
			ac_new->src_rect.x = ac_new->src_rect.w;
			ac_new->sprite_changed = true;
			mouse->cur_target = new_target;
		}
	}
}

void MainMenuSystem::mouse_down_listener()
{
	IMouse* mouse = static_cast<IMouse*>(MainMenuSystem::mouse->get_component(Component::ComponentType::Mouse));
	if (!mouse->cur_target)
		return;

	mouse->down_target = mouse->cur_target;
	IUIElement* target = static_cast<IUIElement*>(mouse->down_target->get_component(Component::ComponentType::UIElement));
	IAnimatable * ac;
	switch (target->element_type)
	{
	case UI_Element_Type::button:
		ac = static_cast<IAnimatable*>(mouse->cur_target->get_component(Component::ComponentType::Animated));
		ac->src_rect.x = ac->src_rect.w * 2;
		ac->sprite_changed = true;
		break;
	case UI_Element_Type::slider_slab:
		break;
	case UI_Element_Type::slider_slide:
		break;
	}
}

void MainMenuSystem::mouse_up_listener()
{
	IMouse* mouse = static_cast<IMouse*>(MainMenuSystem::mouse->get_component(Component::ComponentType::Mouse));
	if (mouse->cur_target = mouse->down_target)
	{
		mouse->down_target = nullptr;
		mouse_clicked_on_entity();
	}
}

void MainMenuSystem::mouse_clicked_on_entity()
{
	IMouse* mouse = static_cast<IMouse*>(MainMenuSystem::mouse->get_component(Component::ComponentType::Mouse));
	Entity* clicked = mouse->cur_target;
	IUIElement* ui = static_cast<IUIElement*>(clicked->get_component(Component::ComponentType::UIElement));
	IAnimatable * ac = static_cast<IAnimatable*>(clicked->get_component(Component::ComponentType::Animated));
	switch (ui->element_type)
	{
	case UI_Element_Type::button:
		ac->src_rect.x = ac->src_rect.w*3;
		ac->sprite_changed = true;
		if (ui->name == "Exit")
		{
			for(unsigned int i = 0; i < exit_listeners.size(); i++)
			{
				void(*f)() = exit_listeners.at(i);
				f();
			}
		}
		else if (ui->name == "New game")
		{
			for (unsigned int i = 0; i < start_listeners.size(); i++)
			{
				void(*f)() = start_listeners.at(i);
				f();
			}
		}
		break;
	}
}

void MainMenuSystem::remove_listeners()
{
	exit_listeners.clear();
	start_listeners.clear();
	load_listeners.clear();
	settings_listeners.clear();
}

MainMenuSystem::MainMenuSystem()
{
}


MainMenuSystem::~MainMenuSystem()
{
}
