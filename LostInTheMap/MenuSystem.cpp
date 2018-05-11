#include "MenuSystem.h"

Entity * MenuSystem::mouse;
void(*MenuSystem::exit)();
void(*MenuSystem::start)();
void(*MenuSystem::resume)();
void(*MenuSystem::back_to_menu)();


void MenuSystem::init_menu(MenuLayout layout, Space & space, menu_type type)
{
#pragma region background
	Entity* background = new Entity(entity_type::background);
	Transform* background_transf = new Transform(background);
	if(type==menu_type::main_menu)
		SDL_manager::get_window_size(&background_transf->position.w, &background_transf->position.h);
	else if (type == menu_type::pause_menu)
	{
		SDL_Rect win_size;
		SDL_manager::get_window_size(&win_size.w, &win_size.h);
		background_transf->position = {win_size.w / 2 - 100, win_size.h / 2 - 150, 200, 300};
	}
	

	IDrawable* background_draw_component = new IDrawable(background, IDrawable::layers::background);
	if (type == menu_type::pause_menu)
		background_draw_component->layer = IDrawable::layers::ui;
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
			
			if(type == main_menu)
				animation->spritesheet = asset_controller::create_ui_text_button_spritesheet(button.name, UI_text_type::main_menu_button_main);
			else if (type == pause_menu)
				animation->spritesheet = asset_controller::create_ui_text_button_spritesheet(button.name, UI_text_type::pause_menu_button);
			SDL_Rect temp = asset_controller::get_texture_size(animation->spritesheet);
			animation->src_rect = { 0,0, temp.w / 5, temp.h };
			element->add_component(animation);

			Transform* button_transf = new Transform(element);
			button_transf->position = { button.position.x, button.position.y, animation->src_rect.w, animation->src_rect.h };
			element->transform = button_transf;

			IDrawable* drawable = new IDrawable(element, IDrawable::layers::surface);
			if (type == menu_type::pause_menu)
				drawable->layer = IDrawable::layers::ui;
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
}

void MenuSystem::update_menu_space(Space& space, int dt, Entity* mouse)
{
	MenuSystem::mouse = mouse;
	mouse_system::update_mouse(mouse, space, true, false);

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
					mouse_target_changed(temp_e, mouse);
				}
			}
			else if (mc->cur_target == temp_e)
				mouse_target_changed(nullptr, mouse);

		}
	}
	//std::cout << dt << std::endl;
	animator::apply_animation_sprite_changes(space);
}

void MenuSystem::mouse_target_changed(Entity* new_target, Entity* mouse_obj)
{
	IMouse* mouse = static_cast<IMouse*>(mouse_obj->get_component(Component::ComponentType::Mouse));

	//if mouse had a target before, update it and remove the reference to it from mouse
	if (mouse->cur_target != nullptr)
	{
		IAnimatable* ac_old = static_cast<IAnimatable*>(mouse->cur_target->get_component(Component::ComponentType::Animated));
		if (ac_old->src_rect.x != 0)
		{
			ac_old->cur_column = 0;
			ac_old->sprite_changed = true;
			mouse->cur_target = nullptr;
		}
	}

	//hovered over button
	if (new_target != nullptr)
	{
		IAnimatable* ac_new = static_cast<IAnimatable*>(new_target->get_component(Component::ComponentType::Animated));
		if (new_target != nullptr)
		{
			ac_new->cur_column = 1;
			ac_new->src_rect.x = ac_new->src_rect.w;
			ac_new->sprite_changed = true;
			mouse->cur_target = new_target;
		}
	}
}

void MenuSystem::mouse_down_listener()
{
	IMouse* mouse = static_cast<IMouse*>(MenuSystem::mouse->get_component(Component::ComponentType::Mouse));
	if (!mouse->cur_target)
		return;

	mouse->down_target = mouse->cur_target;
	IUIElement* target = static_cast<IUIElement*>(mouse->down_target->get_component(Component::ComponentType::UIElement));
	IAnimatable * ac;
	switch (target->element_type)
	{
	case UI_Element_Type::button:
		ac = static_cast<IAnimatable*>(mouse->cur_target->get_component(Component::ComponentType::Animated));
		ac->cur_column = 2;
		ac->sprite_changed = true;
		break;
	case UI_Element_Type::slider_slab:
		break;
	case UI_Element_Type::slider_slide:
		break;
	}
}

void MenuSystem::mouse_up_listener()
{
	IMouse* mouse = static_cast<IMouse*>(MenuSystem::mouse->get_component(Component::ComponentType::Mouse));
	if (mouse->cur_target = mouse->down_target)
	{
		mouse->down_target = nullptr;
		mouse_clicked_on_entity();
	}
}

void MenuSystem::mouse_clicked_on_entity()
{
	IMouse* mouse = static_cast<IMouse*>(MenuSystem::mouse->get_component(Component::ComponentType::Mouse));
	Entity* clicked = mouse->cur_target;
	IUIElement* ui = static_cast<IUIElement*>(clicked->get_component(Component::ComponentType::UIElement));
	IAnimatable * ac = static_cast<IAnimatable*>(clicked->get_component(Component::ComponentType::Animated));
	switch (ui->element_type)
	{
	case UI_Element_Type::button:
		ac->cur_column = 3;
		ac->sprite_changed = true;
		if (ui->name == "Exit")
		{
			exit();
		}
		else if (ui->name == "New game")
		{
			start();
		}
		else if (ui->name == "Resume")
		{
			resume();
		}
		else if (ui->name == "Main Menu")
		{
			back_to_menu();
		}
		break;
	}
}

MenuSystem::MenuSystem()
{
}


MenuSystem::~MenuSystem()
{
}
