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
		IDrawable* comp = static_cast<IDrawable*>(find_component_on_object(*it, ComponentType::Drawable));

		if (comp)
			comp->id = render_system::add_object_to_queue(comp);
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