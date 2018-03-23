#pragma once
#include <SDL.h>
#include <string>
#include <vector>

#include <iostream>
#include "asset_controller.h"

#include "lee_pathfinder.h"


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
	Terrain,
	Tile,
	Character
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

class IDrawable : public Component
{
public:
	enum layers
	{
		background,
		terrain,
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

class ITerrain : public Component
{
public:
	Entity * ** terrain_tiles;
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

class ITile : public Component
{
public:
	std::vector<Entity*> neighbours;
	int x, y;
	bool is_traversible;

	ITile(Entity* owner, int x, int y, bool traversible) : Component(owner)
	{
		this->x = x;
		this->y = y;
		is_traversible = traversible;
		type = ComponentType::Tile;
	}
};

enum character_type
{
	h_giovanni,
	h_zaji,
	zakra_spearman,
	juji_spearman,
	juji_friendly_1,
	juji_friendly_2,
	h_zurshi,
};
class ICharacter : public Component
{
public:
	character_type c_type;

	ICharacter(Entity* owner, character_type type) : Component(owner)
	{
		c_type = type;
		this->type = ComponentType::Character;
		
	}
};

class IMoving : public Component
{
public:
	lee_pathfinder pathfinder;
	SDL_Point final_destination;
	SDL_Point origin;

	bool path_calculated;
	std::vector<SDL_Point> path;

	IMoving(Entity* owner, int owner_t_x, int owner_t_y) : Component(owner)
	{
		origin = { owner_t_x, owner_t_y };
		pathfinder.set_origin({ owner_t_x, owner_t_y });
		path_calculated = false;
		this->type = ComponentType::Movement;
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



class IComposite : public Component
{
public:
	std::string name;
	std::vector<Entity*> dependencies;
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