#pragma once
#include <vector>
#include "Components.h"

enum entity_type
{
	background,
	ui_element,
	mouse,
	tilemap,
	game_object
};

class Entity
{
public:
	Transform* transform;
	std::vector<Component*> components;
	entity_type type;
	std::string name;

	Entity(entity_type type, std::string name="no_name")
	{
		this->type = type;
		this->name = name;
	}

	void add_component(Component* cmp)
	{

		if (cmp->type == Component::ComponentType::Transf)
		{
			if (transform)
				delete transform;
			transform = static_cast<Transform*>(cmp);
			transform->owner = this;
			return;
		}

		bool already_exists = false;
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); it++)
		{
			Component* temp = *it;
			if (temp->type == cmp->type)
			{
				already_exists = true;
				break;
			}
		}

		if (!already_exists)
		{
			components.push_back(cmp);
			cmp->owner = this;
		}
	}

	void remove_component(Component::ComponentType type)
	{
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); it++)
		{
			Component* temp = *it;
			if (temp->type == type)
			{
				components.erase(it);
				break;
			}
		}
	}

	Component* get_component(Component::ComponentType type)
	{
		Component* result = nullptr;
		if (type == Component::ComponentType::Transf)
			return this->transform;

		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); it++)
		{
			Component* temp = *it;
			if (temp->type == type)
			{
				result = *it;
				break;
			}
		}
		return result;
	}

	SDL_Point get_origin_in_world()
	{
		SDL_Point result;
		Transform* tf = this->transform;
		IDrawable* dc = static_cast<IDrawable*>(this->get_component(Component::ComponentType::Drawable));

		if (!tf || !dc)
			return { 0,0 };

		result.x = tf->position.x + dc->sprite_origin.x;
		result.y = tf->position.y + dc->sprite_origin.y;
		return result;
	}

	SDL_Point get_sprite_origin()
	{
		SDL_Point result;
		IDrawable* dc = static_cast<IDrawable*>(this->get_component(Component::ComponentType::Drawable));
		if (!dc)
			return { 0,0 };
		return dc->sprite_origin;
	}

};