#pragma once
#include <vector>
#include <SDL.h>
#include "Entity.h"

enum action_type
{
	not_set,
	movement,
	dialogue,
	wait,
	change_camera_target,
	move_camera_to_tile,
	start_combat
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

	//used if wait
	int time;
};

class Script
{
public:
	bool initialised = false;
	bool blocks_player;
	std::vector<Action> actions;

};
