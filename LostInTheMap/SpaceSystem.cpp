#include "SpaceSystem.h"

void SpaceSystem::add_space_to_render_queue(Space& space)
{
	for (std::vector<Entity*>::iterator it = space.objects.begin(); it != space.objects.end(); it++)
	{
		Entity* temp = *it;
		IDrawable* comp = static_cast<IDrawable*>(temp->get_component(ComponentType::Drawable));

		if (comp)
			comp->id = render_system::add_object_to_queue(comp);
	}
}//function will add all of the drawable components of objects in spaces to render manager

void SpaceSystem::apply_animation_sprite_changes(Space& space)
{
	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		IAnimatable* ac = static_cast<IAnimatable*>(space.objects.at(i)->get_component(ComponentType::Animated));
		if (!ac || !ac->sprite_changed)
			continue;

		IDrawable* dc = static_cast<IDrawable*>(space.objects.at(i)->get_component(ComponentType::Drawable));
		if (!dc)
			continue;

		asset_controller::destroy_texture(dc->sprite);
		dc->sprite = asset_controller::get_sprite_from_spritesheet(ac->spritesheet, ac->src_rect);
		ac->sprite_changed = false;

	}
}

//function clears all memory allocated for entities inside space and their components
void SpaceSystem::destroy_space(Space& space)
{
	for (int i = static_cast<int>(space.objects.size()-1); i >= 0; i--)
	{
		Entity* temp = space.objects.at(i);

		for (int j = static_cast<int>(temp->components.size()-1); j >= 0; j--)
		{
			//remove textures from all of the components
			Component* temp_tc = temp->components.at(j);
			if (temp_tc->type == ComponentType::Drawable)
			{
				IDrawable* temp_c = static_cast<IDrawable*>(temp_tc);
				render_system::remove_from_queue(temp_c->id, temp_c->layer);
				asset_controller::destroy_texture(temp_c->sprite);
			}
			if (temp_tc->type == ComponentType::Animated)
			{
				IAnimatable* temp_c = static_cast<IAnimatable*>(temp_tc);
				asset_controller::destroy_texture(temp_c->spritesheet);
			}
			temp->components.erase(temp->components.begin() + j);
			delete temp_tc;
		}
		delete temp->transform;
		delete temp;
	}

	space.objects.clear();
	space.initialised = false;
}

Entity* SpaceSystem::find_entity_by_name(Space& space, std::string name)
{
	Entity* result = nullptr;
	for(unsigned int i = 0; i < space.objects.size(); i++)
	{
		Entity* temp = space.objects.at(i);
		if (temp->name.compare(name.c_str()) == 0)
			result = temp;
	}

	return result;
}

Entity* SpaceSystem::create_mouse()
{
	Entity* mouse = new Entity(entity_type::mouse);
	Transform* m_transform = new Transform(mouse);
	m_transform->position = { 0,0,1,1 };

	IDrawable* m_draw_comp = new IDrawable(mouse, IDrawable::layers::foreground);
	m_draw_comp->sprite = asset_controller::load_texture("assets/graphics/ui/mouse.png");

	m_draw_comp->draw_rect = asset_controller::get_texture_size(m_draw_comp->sprite);

	IMouse* mc = new IMouse(mouse);

	mouse->transform = m_transform;
	mouse->add_component(m_draw_comp);
	mouse->add_component(mc);

	return mouse;
}

void SpaceSystem::update_mouse(Entity* mouse)
{
	IDrawable* dc = static_cast<IDrawable*>(mouse->get_component(ComponentType::Drawable));
	mouse->transform->position.x = input_system::mouse.x;
	mouse->transform->position.y = input_system::mouse.y;
	dc->draw_rect.x = input_system::mouse.x;
	dc->draw_rect.y = input_system::mouse.y;
}
