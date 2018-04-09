#pragma once
#include <string>

#include "dialogue_system.h"
#include "xml_system.h"
class director
{
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
	
	struct DialogBound
	{
		xml_system::Dialogue dialogue;
		Entity* entity;
	};
	static std::vector<DialogBound> bound;

public:
	static void init_stage() { cur_stage = pyramid_entrance; };
	static void change_stage(story_stage new_stage) { cur_stage = new_stage; }

	static xml_system::Dialogue get_dialogue(Entity* target);

	static void process_interaction(Entity* target);

	static void clear_bound() { secondary_counter = 0; bound.clear(); }

	director();
	~director();
};

