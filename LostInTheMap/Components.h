#pragma once
#include <SDL.h>
#include <string>
#include <vector>

#include <iostream>

enum ComponentType
{
	Animated,
	Collision,
	Description,
	Drawable,
	Fighting,
	Living,
	Location,
	Movement,
	Talking
};

class Entity;

class Component
{
public:
	bool isActive;
	ComponentType type;

	Entity* owner;
	virtual void ReceiveMessage(void* message) = 0;
	virtual ~Component() {};
	Component() {};
};

class IAnimatable : public Component
{
public:
	SDL_Texture* spritesheet;
	SDL_Rect src_rect;
	int cur_x_sprite, cur_y_sprite;
	int time_elapsed;
	int total_sprite_required_time = 100;
	IAnimatable() : Component() {  }
	void ReceiveMessage(void* message) {}

	~IAnimatable() {}
};

class ICollidable : public Component
{
public:
	SDL_Rect collision_rect;
	bool collidable = true;
	void ReceiveMessage(void* message) {}
};

class IDescriptable : public Component
{
public:
	SDL_Texture * box_background;
	SDL_Texture* rendered_text;
	std::string text;
	SDL_Rect box_draw_rect;
	IDescriptable() : Component() {  }
	void ReceiveMessage(void* message) {}
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

	layers layer;
	SDL_Texture* sprite;
	SDL_Rect draw_rect;
	void ReceiveMessage(void* message) {}
	IDrawable() : Component() {  }

	~IDrawable()
	{
		std::cout << "component_destroyed" << std::endl;
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
	void ReceiveMessage(void* message) {}
	IFightable() : Component() {  }
};

class ILiving : public Component
{
public:
	int cur_health;
	int health_max;
	bool is_dead;
	void ReceiveMessage(void* message) {}
	ILiving() : Component() {  }
};

class Transform : public Component
{
public:
	SDL_Rect position;
	void ReceiveMessage(void* message) {}
	Transform() : Component() {  }
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
	void ReceiveMessage(void* message) {}
	IMovable() : Component() {  }
};

class ITalkable : public Component
{
public:
	std::vector<std::string> lines;
	void ReceiveMessage(void* message) {}
	ITalkable() : Component() {  }
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