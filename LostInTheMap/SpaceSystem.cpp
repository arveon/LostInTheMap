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