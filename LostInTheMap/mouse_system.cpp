#include "mouse_system.h"

mouse_system::mouse_icons mouse_system::cur_icon;

Entity* mouse_system::create_mouse()
{
	Entity* mouse = new Entity(entity_type::mouse, "mouse");
	Transform* m_transform = new Transform(mouse);
	m_transform->position = { 0,0,1,1 };
	m_transform->origin = { 0,0 };

	IAnimatable* m_anim_comp = new IAnimatable(mouse);
	m_anim_comp->spritesheet = asset_controller::load_texture("assets/graphics/ui/mouse.png");
	m_anim_comp->src_rect = { 0,0,16,16 };
	m_anim_comp->sprite_changed = false;

	IDrawable* m_draw_comp = new IDrawable(mouse, IDrawable::layers::mouse);
	m_draw_comp->sprite = asset_controller::get_sprite_from_spritesheet(m_anim_comp->spritesheet, m_anim_comp->src_rect);

	m_draw_comp->draw_rect = asset_controller::get_texture_size(m_draw_comp->sprite);

	IMouse* mc = new IMouse(mouse);

	mouse->transform = m_transform;
	mouse->add_component(m_draw_comp);
	mouse->add_component(mc);
	mouse->add_component(m_anim_comp);

	cur_icon = mouse_icons::normal;

	return mouse;
}

void mouse_system::update_mouse(Entity* mouse, Space& space, bool in_dialogue)
{
	IDrawable* dc = static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable));
	IAnimatable* ac = static_cast<IAnimatable*>(mouse->get_component(Component::ComponentType::Animated));
	IMouse* mc = static_cast<IMouse*>(mouse->get_component(Component::ComponentType::Mouse));
	
	mouse->transform->position.x = input_system::mouse.x;
	mouse->transform->position.y = input_system::mouse.y;
	dc->draw_rect.x = input_system::mouse.x;
	dc->draw_rect.y = input_system::mouse.y;

	
	if (!in_dialogue)
	{
		SDL_Point mouse_world = mouse_system::get_mouse_in_world(mouse);
		Entity* target_object = SpaceSystem::get_object_at_point(space, mouse_world.x, mouse_world.y);

		//if target object hasn't changed, just return
		if (target_object == mc->cur_target)
			return;

		if (target_object)
		{
			ITile* tc = static_cast<ITile*>(target_object->get_component(Component::ComponentType::Tile));
			ICharacter* cc = static_cast<ICharacter*>(target_object->get_component(Component::ComponentType::Character));
			if (tc)
				if (tc->is_traversible)
				{
					mc->cur_target = target_object;
					change_mouse_icon(mouse_icons::walking, ac, dc);
				}
				else
				{
					mc->cur_target = nullptr;
					change_mouse_icon(mouse_icons::blocked, ac, dc);
				}
			else if (cc)
			{
				mc->cur_target = target_object;
				if (cc->is_friendly)
					change_mouse_icon(mouse_icons::talk, ac, dc);
				else
					change_mouse_icon(mouse_icons::attack, ac, dc);
			}
		}
		else
		{
			mc->cur_target = nullptr;
			change_mouse_icon(mouse_icons::blocked, ac, dc);
		}
	}
	else
	{
		change_mouse_icon(mouse_icons::normal, ac, dc);
	}

}

SDL_Point mouse_system::get_mouse_in_world(Entity * mouse)
{
	IMouse* ms = static_cast<IMouse*>(mouse->get_component(Component::ComponentType::Mouse));
	//get mouse screen space
	SDL_Point mouse_pos = { mouse->transform->position.x + mouse->transform->origin.x, mouse->transform->position.y + mouse->transform->origin.y };
	//get mouse world space
	mouse_pos = camera_system::screen_to_world_space(mouse_pos);

	return mouse_pos;
}

void mouse_system::change_mouse_icon(mouse_icons icon, IAnimatable* anim_component, IDrawable* draw_component)
{
	if (cur_icon == icon)
		return;
	Transform* tf = static_cast<Transform*>(anim_component->owner->get_component(Component::ComponentType::Transf));
	if (!tf)
		return;
	switch (icon)
	{
	case mouse_icons::normal:
		cur_icon = mouse_icons::normal;
		tf->origin = { 0,0 };
		anim_component->src_rect.x = 0;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 16;
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::walking:
		cur_icon = mouse_icons::walking;
		tf->origin = { tf->position.w / 2, tf->position.h / 2 };
		anim_component->src_rect.x = 16;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 24;
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::attack:
		cur_icon = mouse_icons::attack;
		tf->origin = { tf->position.w / 2, tf->position.h / 2 };
		anim_component->src_rect.x = 32;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 32;
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::blocked:
		cur_icon = mouse_icons::blocked;
		tf->origin = { tf->position.w / 2, tf->position.h / 2 };
		anim_component->src_rect.x = 48;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 24;
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::talk:
		cur_icon = mouse_icons::talk;
		tf->origin = { tf->position.w / 2, tf->position.h / 2 };
		anim_component->src_rect.x = 64;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 32;
		anim_component->sprite_changed = true;
		break;
	}


}

void mouse_system::update_mouse_hover(Space& space, Entity* mouse)
{

}


mouse_system::mouse_system()
{
}


mouse_system::~mouse_system()
{
}
