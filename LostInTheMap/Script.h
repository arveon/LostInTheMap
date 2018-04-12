#pragma once
#include <vector>
#include <SDL.h>
#include "Entity.h"

enum action_type
{
	not_set,
	movement,
	dialogue
};

class Action
{
public:
	action_type type;

	//used if movement
	SDL_Point movement_dest;
	Entity* target;
	character_type target_type;

	//used if dialogue
	std::string dialogue_path;
	bool finished = false;
};

class Script
{
public:
	bool blocks_player;
	std::vector<Action> actions;

};
