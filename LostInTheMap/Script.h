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
	start_combat,
	set_story_state,
	object_disappear,
	character_disappear,
	level_switch,
	join_army
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

	//utility stuff
	std::string utility;
	int num_utility;
};

class Script
{
public:
	bool initialised = false;
	bool happens_once = false;
	bool blocks_player;
	std::vector<Action> actions;

};
