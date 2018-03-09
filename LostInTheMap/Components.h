#pragma once
#include <SDL.h>
#include <string>
#include <vector>

#include <iostream>
#include "asset_controller.h"


enum ComponentType
{
	Animated,
	Collision,
	Description,
	Drawable,
	Fighting,
	Living,
	Transf,
	Movement,
	Talking,
	Mouse,
	UIElement,
	Terrain
};

class Entity;

class Component
{
public:
	bool isActive;
	ComponentType type;

	Entity* owner;
	virtual ~Component() {};
	Component(Entity* owner) { this->owner = owner; }
};

class IAnimatable : public Component
{
public:
	SDL_Texture* spritesheet;
	SDL_Rect src_rect;
	//int cur_x_sprite, cur_y_sprite;
	int time_elapsed;
	int total_sprite_required_time = 100;
	bool sprite_changed;
	IAnimatable(Entity* owner) : Component(owner) 
	{  
		src_rect = { 0,0,1,1 };
		sprite_changed = false;
		time_elapsed = 0;
		type = ComponentType::Animated;
		isActive = true;
	}

	~IAnimatable() {}
};

class ICollidable : public Component
{
public:
	SDL_Rect collision_rect;//relative to transform
	bool collidable = true;
	ICollidable(Entity* owner, int width, int height) : Component(owner)
	{
		type = ComponentType::Collision;
		collision_rect = { 0,0,width,height };
	}
};

class IDescriptable : public Component
{
public:
	SDL_Texture * box_background;
	SDL_Texture* rendered_text;
	std::string text;
	SDL_Rect box_draw_rect;
	IDescriptable(Entity* owner) : Component(owner)
	{
		type = ComponentType::Description;
		isActive = true;
	}
};

class IDrawable : public Component
{
public:
	enum layers
	{
		background,
		surface,
		foreground,
		ui
	};

	int id;
	layers layer;
	SDL_Texture* sprite;
	SDL_Rect draw_rect;
	IDrawable(Entity* owner, layers layer) : Component(owner)
	{
		id = 0;
		this->layer = layer;
		type = ComponentType::Drawable;
		isActive = true;
	}

	~IDrawable()
	{
		//asset_controller::destroy_texture(sprite);
	}
};


enum unit_type
{
	//TODO: list all the possible units that will be encountered in the game


};

struct unit
{
	unit_type type;
	int quantity;
	int health_of_first; //remaining health of the first unit in stack
};

class IFightable : public Component
{
public:
	unit army[6]; //can be up to 6 different units in army
	IFightable(Entity* owner) : Component(owner)
	{
		type = ComponentType::Fighting;
		isActive = true;
	}
};

class ILiving : public Component
{
public:
	int cur_health;
	int max_health;
	bool is_dead;
	ILiving(Entity* owner, int max_health) : Component(owner)
	{
		cur_health = max_health;
		type = ComponentType::Living;
		isActive = true;
	}
};

class Transform : public Component
{
public:
	SDL_Rect position;
	Transform(Entity* owner, int x = 0, int y = 0) : Component(owner)
	{
		position.x = x;
		position.y = y;
		type = ComponentType::Transf;
		isActive = true;
	}
};

struct tile
{
	int x, y;
};

class IMovable : public Component
{
public:
	float speed;
	bool allowed_movement;
	std::vector<tile> path;
	IMovable(Entity* owner) : Component(owner)
	{
		type = ComponentType::Movement;
		isActive = true;
	}
};

class ITalkable : public Component
{
public:
	std::vector<std::string> lines;
	ITalkable(Entity* owner) : Component(owner)
	{
		ComponentType::Talking;
		isActive = true;
	}
};

enum InteractionState 
{
	none,
	hover,
	pressed,
	blocked
};
class IMouseInteractable : public Component
{
public:
	InteractionState state;
};

class IComposite : public Component
{
public:
	std::string name;
	std::vector<Entity*> dependencies;
};

enum UI_Element_Type
{
	button,
	slider_slide,
	slider_slab,
	text,
	loading_bar
};
class IUIElement : public Component
{
public:
	UI_Element_Type element_type;
	std::string name;
	IUIElement(Entity* owner) : Component(owner)
	{
		type = ComponentType::UIElement;
		isActive = true;
	}
};

class IMouse : public Component
{
public:
	Entity * cur_target;
	Entity * down_target;
	IMouse(Entity* owner) : Component(owner) 
	{
		type = ComponentType::Mouse;
		isActive = true;
		cur_target = nullptr;
		down_target = nullptr;
	}
};

class ISlider : public Component
{
public:
	Entity * slide;
	Entity * slab;
	int value;
	ISlider(Entity* owner, Entity* slide, Entity* slab, int value) : Component(owner)
	{
		isActive = true;
		this->slide = slide;
		this->slab = slab;
		this->value = value;
	}

};

class ITerrain : public Component
{
public:
	Entity*** terrain_tiles;
	int width;
	int height;
	int tile_width;
	ITerrain(Entity* owner) : Component(owner)
	{
		width = 0;
		height = 0; 
		terrain_tiles = nullptr;
		type = ComponentType::Terrain;
	}


};