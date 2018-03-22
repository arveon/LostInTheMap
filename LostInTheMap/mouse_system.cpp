#include "mouse_system.h"


Entity* mouse_system::create_mouse()
{
	Entity* mouse = new Entity(entity_type::mouse, "mouse");
	Transform* m_transform = new Transform(mouse);
	m_transform->position = { 0,0,1,1 };

	IAnimatable* m_anim_comp = new IAnimatable(mouse);
	m_anim_comp->spritesheet = asset_controller::load_texture("assets/graphics/ui/mouse.png");
	m_anim_comp->src_rect = { 0,0,16,16 };
	m_anim_comp->sprite_changed = false;

	IDrawable* m_draw_comp = new IDrawable(mouse, IDrawable::layers::foreground);
	m_draw_comp->sprite = asset_controller::get_sprite_from_spritesheet(m_anim_comp->spritesheet, m_anim_comp->src_rect);

	m_draw_comp->draw_rect = asset_controller::get_texture_size(m_draw_comp->sprite);

	IMouse* mc = new IMouse(mouse);

	mouse->transform = m_transform;
	mouse->add_component(m_draw_comp);
	mouse->add_component(mc);
	mouse->add_component(m_anim_comp);

	return mouse;
}

void mouse_system::update_mouse(Entity* mouse)
{
	IDrawable* dc = static_cast<IDrawable*>(mouse->get_component(ComponentType::Drawable));
	mouse->transform->position.x = input_system::mouse.x;
	mouse->transform->position.y = input_system::mouse.y;
	dc->draw_rect.x = input_system::mouse.x;
	dc->draw_rect.y = input_system::mouse.y;
}

void mouse_system::change_mouse_icon(mouse_icons icon, IAnimatable* anim_component, IDrawable* draw_component)
{
	
	switch (icon)
	{
	case mouse_icons::normal:
		anim_component->src_rect.x = 0;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 16;
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::walking:
		anim_component->src_rect.x = 16;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 16;
		anim_component->sprite_changed = true;
		break;
	}


}


mouse_system::mouse_system()
{
}


mouse_system::~mouse_system()
{
}
