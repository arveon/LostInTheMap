#pragma once
#include <vector>
#include "Components.h"

enum entity_type
{
	background,
	ui_element,
	mouse

};

class Entity
{
public:
	Transform* transform;
	std::vector<Component*> components;
	entity_type type;

	void add_component(Component* cmp)
	{

		if (cmp->type == ComponentType::Transf && transform == nullptr)
		{
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

	void remove_component(ComponentType type)
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

	Component* get_component(ComponentType type)
	{
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); it++)
		{
			Component* temp = *it;
			if (temp->type == type)
			{
				return *it;
			}
		}


	}

};