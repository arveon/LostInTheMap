#pragma once
#include <string>

#include "dialogue_system.h"
#include "script_system.h"
#include "xml_system.h"
#include "Script.h"
#include "Levels.h"
#include "StoryStages.h"
class director
{
public:
	enum enemy
	{
		rat_1,
		rat_2,
		rat_3,
		rat_4,
		rat_5
	};
private:
	
	static bool level_switch_pending;

	static story_stage cur_stage;
	static int secondary_counter;
	
	static Space* space;

	struct DialogBound
	{
		xml_system::Dialogue dialogue;
		Entity* entity;
	};
	static std::vector<DialogBound> bound;
	static std::vector<std::string> triggered_scripts;
	
	
public:
	static levels cur_level;
	static levels target_level;
	static void init_stage(levels level);
	static void change_stage(story_stage new_stage) { cur_stage = new_stage; }

	static void set_space(Space* game_space) { space = game_space; }

	static xml_system::Dialogue get_dialogue(Entity* target);
	static void start_scripted_dialogue(std::string path);

	static void process_interaction(Entity* target);
	static void script_trigger(Entity* trigger);
	//used to initialise scripts that always happen inside any level (eg. level start script)
	static void script_trigger(std::string path);

	static void set_story_state(std::string new_state);

	static story_stage get_story_state_from_name(std::string name);

	static void set_level_switch(levels target) { level_switch_pending = true; target_level = target; }

	static void reset_director();

	static bool is_level_switch_pending() { return level_switch_pending; }

	director();
	~director();
	
	static xml_system::Dialogue pyramid_entrance_dialogues(Entity * target);
	static xml_system::Dialogue pyramid_gold_picked_up_dialogues(Entity * target);
	static xml_system::Dialogue pyramid_gold_taken_dialogues(Entity * target);
	static xml_system::Dialogue get_archaeologist_dialogue(Entity * target, ICharacter* cc);
};

