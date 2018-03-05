#include "SpaceSystem.h"

Component* SpaceSystem::find_component_on_object(Entity *ent, ComponentType type)
{
	Component* required_comp = nullptr;

	if (type == ComponentType::Transf)
		return ent->transform;
	for (std::vector<Component*>::iterator c_it = ent->components.begin(); c_it != ent->components.end(); c_it++)
	{
		if ((*c_it)->type == type)
		{
			required_comp = *c_it;
			break;
		}
	}
	return required_comp;
}

void SpaceSystem::add_space_to_render_queue(Space& space)
{
	for (std::vector<Entity*>::iterator it = space.objects.begin(); it != space.objects.end(); it++)
	{
		Component* comp = find_component_on_object(*it, ComponentType::Drawable);

		if (comp)
			render_system::add_object_to_queue(static_cast<IDrawable*>(comp));
	}
}//function will add all of the drawable components of objects in spaces to render manager

void SpaceSystem::apply_animation_sprite_changes(Space& space)
{
	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		IAnimatable* ac = static_cast<IAnimatable*>(find_component_on_object(space.objects.at(i), ComponentType::Animated));
		if (!ac)
			continue;

		if (!ac->sprite_changed)
			continue;

		IDrawable* dc = static_cast<IDrawable*>(find_component_on_object(space.objects.at(i), ComponentType::Drawable));
		if (!dc)
			continue;

		asset_controller::destroy_texture(dc->sprite);
		dc->sprite = asset_controller::get_sprite_from_spritesheet(ac->spritesheet, ac->src_rect);
		ac->sprite_changed = false;

	}
}