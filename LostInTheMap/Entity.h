#pragma once
#include <vector>
#include "Components.h"

enum entity_type
{
	background,
	ui_element,
	mouse,
	tilemap,
	game_object,
	game_object_combat,
	trigger
};

class Entity
{
public:
	Transform * transform;
	std::vector<Component*> components;
	entity_type type;
	std::string name;
	bool is_active;

	Entity(entity_type type, std::string name = "no_name")
	{
		this->type = type;
		this->name = name;
		is_active = true;
	}

	void deactivate()
	{
		is_active = true;
		transform->isActive = false;
		for (Component* c : components)
			c->isActive = false;
	}

	void activate()
	{
		is_active = true;
		transform->isActive = true;
		for (Component* c : components)
			c->isActive = true;
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

	//returns requested component or nullptr
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

	//gets objects origin in world or {-1,-1} if transform is missing
	SDL_Point get_origin_in_world()
	{
		SDL_Point result;
		Transform* tf = this->transform;
		IDrawable* dc = static_cast<IDrawable*>(this->get_component(Component::ComponentType::Drawable));

		if (!tf || !dc)
			return { -1,-1 };

		result.x = tf->position.x + tf->origin.x;
		result.y = tf->position.y + tf->origin.y;
		return result;
	}

	//gets sprite origin in local sprite coordinates or {-1,-1} if no transform found
	SDL_Point get_sprite_origin()
	{
		IDrawable* dc = static_cast<IDrawable*>(this->get_component(Component::ComponentType::Drawable));
		if (!dc)
			return { -1,-1 };
		return dc->sprite_origin;
	}

	//gets transform origin in local transform coordinate system or {-1,-1} if no transform found
	SDL_Point get_object_origin()
	{
		Transform* tc = static_cast<Transform*>(this->get_component(Component::ComponentType::Transf));
		if (!tc)
			return { -1,-1 };
		return tc->origin;
	}

	//gets collision origin in world coordinate system or {-1,-1} if no collider found
	SDL_Point get_collision_origin_in_world()
	{
		SDL_Point result;
		Transform* tf = this->transform;
		ICollidable* cc = static_cast<ICollidable*>(this->get_component(Component::ComponentType::Collision));

		if (!tf || !cc)
			return { -1,-1 };

		result.x = tf->position.x + cc->collision_rect.x + cc->collision_rect.w/2;
		result.y = tf->position.y + cc->collision_rect.y + cc->collision_rect.h/2;
		return result;
	}

};