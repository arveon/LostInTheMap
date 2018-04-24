#pragma once
#include <SDL.h>
#include <string>
#include <vector>

#include <iostream>
#include "asset_controller.h"

#include "lee_pathfinder.h"
#include "CharacterTypes.h"
#include "ObjectTypes.h"
#include "ArmyUnit.h"

typedef struct
{
	int value;
	std::string type;
	std::string script;
	std::string army;
} Actor;

class Entity;
class Component
{
public:
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
		Character,
		InteractionSource,
		CombatUnit
	};
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
	SDL_Point origin;
	
	Transform(Entity* owner, int x = 0, int y = 0, SDL_Point origin = {0,0}) : Component(owner)
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
		ui,
		mouse
	};

	int id;
	layers layer;
	SDL_Texture* sprite;
	SDL_Rect draw_rect;
	SDL_Point sprite_origin;
	bool flipped_x;
	IDrawable(Entity* owner, layers layer) : Component(owner)
	{
		id = 0;
		sprite_origin = { 0,0 };
		this->layer = layer;
		type = ComponentType::Drawable;
		isActive = true;
		flipped_x = false;
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
	SDL_Rect collision_rect;//relative to transform?
	bool collidable = true;
	ICollidable(Entity* owner, int width = 0, int height = 0) : Component(owner)
	{
		type = ComponentType::Collision;
		collision_rect = { 0,0,width,height };
		isActive = true;
	}
};

class IInteractionSource : public Component
{	
public:
	Entity * interaction_target;
	bool has_triggered;
	std::string script_attached;

	object_types o_type;

	void(*interaction_trigger)(Entity* source);
	IInteractionSource(Entity* owner, object_types o_type = object_types::type_none) : Component(owner)
	{
		this->o_type = o_type;
		has_triggered = false;
		interaction_target = nullptr;
		type = ComponentType::InteractionSource;
	}
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
	int width;//in tiles
	int height;//in tiles
	int tile_width;
	int tile_height;
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
	int x, y;//grid coordinates of tile
	bool is_traversible;

	ITile(Entity* owner, int x, int y, bool traversible) : Component(owner)
	{
		this->x = x;
		this->y = y;
		is_traversible = traversible;
		type = ComponentType::Tile;
	}
};

class ICharacter : public Component
{
public:
	character_type c_type;
	bool is_friendly;

	bool controlled_by_script;
	void(*script_done_callback)(Entity* script_target);
	ICharacter(Entity* owner, character_type type, bool is_friendly) : Component(owner)
	{
		this->is_friendly = is_friendly;
		c_type = type;
		this->type = ComponentType::Character;
		controlled_by_script = false;
		
	}
};

class IMoving : public Component
{
public:
	lee_pathfinder pathfinder;
	//destination for transform (top left pixel)
	SDL_Point final_destination;
	bool destination_reached;
	//SDL_Point origin;

	float shift_buffer_x;
	float shift_buffer_y;

	bool path_calculated;
	std::vector<SDL_Point> path;

	bool movement_allowed;

	IMoving(Entity* owner, int owner_t_x, int owner_t_y) : Component(owner)
	{
		//origin = { owner_t_x, owner_t_y };
		pathfinder.set_origin({ owner_t_x, owner_t_y });
		path_calculated = false;
		destination_reached = true;
		this->type = ComponentType::Movement;
		movement_allowed = true;

		shift_buffer_x = 0;
		shift_buffer_y = 0;
	}

};

class IFightable : public Component
{
public:
	std::vector<army_unit> army;//can be up to 6 different units in army
	std::string army_file;
	bool friendly;
	IFightable(Entity* owner, bool friendly = false) : Component(owner)
	{
		type = ComponentType::Fighting;
		isActive = true;
		this->friendly = friendly;
	}
};

class ICombatUnit : public Component
{
public:
	army_unit unit_stats;

	ICombatUnit(Entity* owner, army_unit unit) : Component(owner)
	{
		type = ComponentType::CombatUnit;
		unit_stats = unit;
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