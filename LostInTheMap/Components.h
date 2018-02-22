#pragma once
#include <SDL.h>
#include <string>
#include <vector>

enum ComponentType
{
	Animated,
	Collision,
	Description,
	Drawing,
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

class IAnimatable : Component
{
public:
	SDL_Texture* spritesheet;
	int cur_x_sprite, cur_y_sprite;
	int time_elapsed;
	int total_sprite_required_time = 100;
	IAnimatable() : Component() {  }
};

class ICollidable : Component
{
public:
	SDL_Rect collision_rect;
	bool collidable = true;

};

class IDescriptable : Component
{
public:
	SDL_Texture * box_background;
	SDL_Texture* rendered_text;
	std::string text;
	SDL_Rect box_draw_rect;

};

class IDrawable : Component
{
	SDL_Texture* sprite;
	SDL_Rect draw_rect;

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

class IFightable : Component
{
public:
	unit army[6]; //can be up to 6 different units in army
};

class ILiving : Component
{
public:
	int cur_health;
	int health_max;
	bool is_dead;

};

class Transform
{
public:
	SDL_Rect position;

};

struct tile
{
	int x, y;
};

class IMovable : Component
{
public:
	float speed;
	bool allowed_movement;
	std::vector<tile> path;

};

class ITalkable : Component
{
	std::vector<std::string> lines;
};