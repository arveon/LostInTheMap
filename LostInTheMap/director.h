#pragma once
#include <string>

#include "dialogue_system.h"
#include "script_system.h"
#include "xml_system.h"
#include "Script.h"
#include "Levels.h"
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
	enum story_stage
	{
		pyramid_entrance,
		pyramid_gold_picked_up,
		pyramid_lever_switched,
		juji_start,
		juji_snakes_start,
		juji_snakes_end,
		bodah_start,
		bodah_end,
		zakra_start,
		zakra_after_leader,
		zakra_after_collected,
		zakra_after_potion,
		zakra_end,
		desert_before_fight,
		desert_zaji_dies,
		desert_zaji_lives,
		end
	};	
	
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
	static void init_stage(levels level);
	static void change_stage(story_stage new_stage) { cur_stage = new_stage; }

	static void set_space(Space* game_space) { space = game_space; }

	static xml_system::Dialogue get_dialogue(Entity* target);
	static void start_scripted_dialogue(std::string path);

	static void process_interaction(Entity* target);
	static void script_trigger(Entity* trigger);

	static void reset_director();

	director();
	~director();
};

