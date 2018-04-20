#pragma once
#include "Script.h"
#include "SpaceSystem.h"
#include "character_system.h"
#include "Components.h"
#include "camera_system.h"
#include "StoryStages.h"

class script_system
{
private:
	static Script cur_script;
	static int cur_action;

	static int waiting_timer;
	static int total_wait_time;

	static void(*combat_start_callback)(levels level, Space&, IFightable*);
	static void(*start_dialogue_callback)(std::string);
	static void(*state_change_callback)(std::string);
public:
	static Space* game_space;

	static bool is_player_blocked() { return (is_script_going() && cur_script.blocks_player); }
	static bool is_script_going();

	static void start_script(Script s);
	static void perform_action();

	
	static void action_over(Entity* action_performer);

	static void update(int dt);

	static void reset();

	static void set_combat_start_callback(void(*combat_callback)(levels, Space&, IFightable*)) { combat_start_callback = combat_callback; }
	static void set_dialogue_start_callback(void(*cb)(std::string)) { start_dialogue_callback = cb; }
	static void set_state_change_callback(void(*cb)(std::string)) { state_change_callback = cb; }

	script_system();
	~script_system();
};

