#include "SpaceSystem.h"

void SpaceSystem::add_space_to_render_queue(Space& space)
{
	for (std::vector<Entity*>::iterator it = space.objects.begin(); it != space.objects.end(); it++)
	{
		Entity* temp = *it;
		IDrawable* comp = static_cast<IDrawable*>(temp->get_component(Component::ComponentType::Drawable));

		if (comp)
			comp->id = render_system::add_object_to_queue(comp);
	}
}//function will add all of the drawable components of objects in spaces to render manager

void SpaceSystem::apply_animation_sprite_changes(Space& space)
{
	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		IAnimatable* ac = static_cast<IAnimatable*>(space.objects.at(i)->get_component(Component::ComponentType::Animated));
		if (!ac || !ac->sprite_changed)
			continue;

		IDrawable* dc = static_cast<IDrawable*>(space.objects.at(i)->get_component(Component::ComponentType::Drawable));
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
	for (int i = static_cast<int>(space.objects.size() - 1); i >= 0; i--)
	{
		Entity* temp = space.objects.at(i);

		for (int j = static_cast<int>(temp->components.size() - 1); j >= 0; j--)
		{
			//remove textures from all of the components
			Component* temp_tc = temp->components.at(j);
			if (temp_tc->type == Component::ComponentType::Drawable)
			{
				IDrawable* temp_c = static_cast<IDrawable*>(temp_tc);
				render_system::remove_from_queue(temp_c->id, temp_c->layer);
				asset_controller::destroy_texture(temp_c->sprite);
			}
			if (temp_tc->type == Component::ComponentType::Animated)
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
	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		Entity* temp = space.objects.at(i);
		if (temp->name.compare(name.c_str()) == 0)
			result = temp;
	}

	return result;
}

Entity* SpaceSystem::get_object_at_point(Space& space, int x, int y)
{
	Entity* ent = nullptr;

	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		Entity* temp = space.objects.at(i);
		if (temp->name.compare("mouse") == 0 || temp->name.compare("player") == 0)
			continue;
		Transform* tf = static_cast<Transform*>(temp->get_component(Component::ComponentType::Transf));
		if (!tf)
			continue;
		SDL_Rect rect = tf->position;
		if (geometry_utilities::is_point_in_rect(x, y, rect))
		{
			ent = temp;
			break;
		}
	}
	if (!ent)
	{
		Entity* terr = SpaceSystem::find_entity_by_name(space, "terrain");
		ITerrain* tc = static_cast<ITerrain*>(terr->get_component(Component::ComponentType::Terrain));

		SDL_Point ids = map_system::world_to_tilemap_ids({ x, y }, tc);
		ent = tc->terrain_tiles[ids.y][ids.x];
	}

	return ent;
}

void SpaceSystem::update_draw_rects(Space & space)
{
	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		Transform* tf = static_cast<Transform*>(space.objects.at(i)->get_component(Component::ComponentType::Transf));
		IDrawable* dc = static_cast<IDrawable*>(space.objects.at(i)->get_component(Component::ComponentType::Drawable));

		IMouse* mc = static_cast<IMouse*>(space.objects.at(i)->get_component(Component::ComponentType::Mouse));
		if (!tf || !dc || mc)
			continue;

		int delta_x = (dc->draw_rect.w - tf->position.w) / 2;
		int delta_y = (dc->draw_rect.h - tf->position.h);

		dc->draw_rect.x = tf->position.x - delta_x;
		dc->draw_rect.y = tf->position.y - delta_y;

	}


}



