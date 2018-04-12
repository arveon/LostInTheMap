#pragma once
#include "Script.h"
#include "SpaceSystem.h"
#include "character_system.h"
#include "Components.h"

class script_system
{
private:
	static Script cur_script;
	static int cur_action;
public:
	static Space* game_space;
	static void(*start_dialogue_callback)(std::string);

	static bool is_player_blocked() { return cur_script.blocks_player; }
	static bool is_script_going() { return (!cur_script.actions.back().finished); }

	static void start_script(Script s);
	static void perform_action();

	static void set_dialogue_start_callback(void(*cb)(std::string)) { start_dialogue_callback = cb; }
	
	static void action_over(Entity* action_performer);

	script_system();
	~script_system();
};

