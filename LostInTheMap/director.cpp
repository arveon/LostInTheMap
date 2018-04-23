#include "director.h"

story_stage director::cur_stage;
int director::secondary_counter;
std::vector<director::DialogBound> director::bound;
std::vector<std::string> director::triggered_scripts;
levels director::cur_level;
Space* director::space;

bool director::level_switch_pending = false;
levels director::target_level;


xml_system::Dialogue director::get_dialogue(Entity * target)
{
	xml_system::Dialogue result;
	switch (cur_stage)
	{
	case pyramid_entrance:
		result = director::pyramid_entrance_dialogues(target);
		break;
	case story_stage::pyramid_gold_picked_up:
		result = director::pyramid_gold_picked_up_dialogues(target);
		break;
	case story_stage::pyramid_gold_taken:
	case story_stage::pyramid_lever_switched:
	case story_stage::pyramid_map_discovered:
		result = director::pyramid_gold_taken_dialogues(target);
		break;
	case story_stage::juji_start:
		result = director::juji_start_dialogues(target);
		break;
	}

	return result;
}

void director::process_interaction(Entity* interaction_target)
{
	if (interaction_target->name.compare("player") == 0)
		return;

	IInteractionSource* interaction_src = static_cast<IInteractionSource*>(interaction_target->get_component(Component::ComponentType::InteractionSource));
	//this block engages dialogue if it's a friendly character (to avoid having to write script that starts a dialogue for every npc)
	ICharacter* target_char = static_cast<ICharacter*>(interaction_target->get_component(Component::ComponentType::Character));
	if (target_char && target_char->is_friendly)
	{
		if (target_char->is_friendly)
		{
			//stop target movement
			
			character_system::stop_character_movement(interaction_target);

			xml_system::Dialogue dial = director::get_dialogue(interaction_target);
			if (dial.lines.size() == 0)
				return;

			std::vector<asset_controller::CharacterPortrait> portraits = asset_controller::get_characters_portraits(dial.characters);
			dialogue_system::start_dialogue(dial, portraits, interaction_target);
		}
		
	}
	else if(interaction_src)
	{
		if (!interaction_src->has_triggered && interaction_src->script_attached != "")
		{//if object wasn't triggered yet and has a script attached, call interaction
			script_trigger(interaction_target);
		}
	}

}

void director::start_scripted_dialogue(std::string path)
{
	xml_system::Dialogue d = xml_system::load_dialogue(cur_level, path);
	d.requires_callback = true;
	d.callback = script_system::action_over;
	std::vector<asset_controller::CharacterPortrait> portraits = asset_controller::get_characters_portraits(d.characters);

	dialogue_system::start_dialogue(d, portraits, nullptr);
}

//calls script stored on trigger object when called
void director::script_trigger(Entity* trigger)
{
	IInteractionSource* is = static_cast<IInteractionSource*>(trigger->get_component(Component::ComponentType::InteractionSource));
	if (is)
	{
		//check if script was called already
		for (std::string temp : triggered_scripts)
		{
			if (temp.compare(is->script_attached) == 0)
				return;
		}

		Script script = xml_system::load_script(is->script_attached, cur_level);
		if(script.initialised)
			script_system::start_script(script);
		if (script.happens_once)
		{
			is->has_triggered = true;
			triggered_scripts.push_back(is->script_attached);
		}
	}
	else
		std::cout << "no is?!" << std::endl;
}

void director::script_trigger(std::string path)
{
	Script script = xml_system::load_script(path, cur_level);
	if (script.initialised)
		script_system::start_script(script);
	if (script.happens_once)
	{
		triggered_scripts.push_back(path);
	}

}

void director::set_story_state(std::string new_state)
{
	cur_stage = get_story_state_from_name(new_state);
}

story_stage director::get_story_state_from_name(std::string name)
{
	story_stage stage;
	if (name.compare("pyramid_entrance") == 0)
		stage = story_stage::pyramid_entrance;
	else if (name.compare("pyramid_gold_picked_up") == 0)
		stage = story_stage::pyramid_gold_picked_up;
	else if (name.compare("pyramid_lever_switched") == 0)
		stage = story_stage::pyramid_lever_switched;
	else if (name.compare("pyramid_map_discovered") == 0)
		stage = story_stage::pyramid_map_discovered;
	else if (name.compare("juji_start") == 0)
		stage = story_stage::juji_start;
	else if (name.compare("juji_snakes_start") == 0)
		stage = story_stage::juji_snakes_start;
	else if (name.compare("juji_snakes_end") == 0)
		stage = story_stage::juji_snakes_end;
	else if (name.compare("bodah_start") == 0)
		stage = story_stage::bodah_start;
	else if (name.compare("bodah_end") == 0)
		stage = story_stage::bodah_end;
	else if (name.compare("zakra_start") == 0)
		stage = story_stage::zakra_start;
	else if (name.compare("zakra_after_collected") == 0)
		stage = story_stage::zakra_after_collected;
	else if (name.compare("zakra_after_potion") == 0)
		stage = story_stage::zakra_after_potion;
	else if (name.compare("zakra_end") == 0)
		stage = story_stage::zakra_end;
	else if (name.compare("desert_before_fight") == 0)
		stage = story_stage::desert_before_fight;
	else if (name.compare("desert_zaji_dies") == 0)
		stage = story_stage::desert_zaji_dies;
	else if (name.compare("desert_zaji_lives") == 0)
		stage = story_stage::desert_zaji_lives;
	else if (name.compare("end") == 0)
		stage = story_stage::end;

	return stage;
}

void director::reset_director()
{
	secondary_counter = 0; 
	level_switch_pending = false;
	bound.clear(); 
	triggered_scripts.clear();

	dialogue_system::reset();
	script_system::reset();
}

void director::init_stage(levels level)
{
	director::cur_level = level;
	script_system::set_dialogue_start_callback(&director::start_scripted_dialogue);
	script_system::set_state_change_callback(&director::set_story_state);
	script_system::set_level_switch_callback(&director::set_level_switch);
}

void director::change_stage(story_stage new_stage)
{
	cur_stage = new_stage;
	//when switching to another story stage, some triggers/scripts should be ignored (e.g. level locks)
	switch (new_stage)
	{
	case story_stage::bodah_start:
		add_ignored_script("bodah_lock.xml");
		break;
	case story_stage::zakra_start:
		add_ignored_script("zakra_lock.xml");
		break;
	}
}

director::director()
{}


director::~director()
{}

xml_system::Dialogue director::pyramid_entrance_dialogues(Entity* target)
{
	xml_system::Dialogue result;
	ICharacter* cc = static_cast<ICharacter*>(target->get_component(Component::ComponentType::Character));
	if (!cc)
		return xml_system::Dialogue();
	switch (cc->c_type)
	{
	case character_type::npc_arch_supervisor:
		result = xml_system::load_dialogue(cur_level, "supervisor_1.xml");
		break;
	case character_type::npc_archaeologist_1:
	case character_type::npc_archaeologist_2:
	case character_type::npc_archaeologist_3:
	case character_type::npc_archaeologist_4:
		result = get_secondary_dialogue(target, cc, "archaeologist");
	}
	return result;
}

xml_system::Dialogue director::pyramid_gold_picked_up_dialogues(Entity* target)
{
	xml_system::Dialogue result;
	ICharacter* cc = static_cast<ICharacter*>(target->get_component(Component::ComponentType::Character));
	if (!cc)
		return xml_system::Dialogue();
	switch (cc->c_type)
	{
	case character_type::npc_arch_supervisor:
		result = xml_system::load_dialogue(cur_level, "supervisor_2.xml");
		cur_stage = story_stage::pyramid_gold_taken;
		break;
	case character_type::npc_archaeologist_1:
	case character_type::npc_archaeologist_2:
	case character_type::npc_archaeologist_3:
	case character_type::npc_archaeologist_4:
		result = get_secondary_dialogue(target, cc, "archaeologist");
	}
	return result;
}

xml_system::Dialogue director::pyramid_gold_taken_dialogues(Entity* target)
{
	xml_system::Dialogue result;
	ICharacter* cc = static_cast<ICharacter*>(target->get_component(Component::ComponentType::Character));
	if (!cc)
		return xml_system::Dialogue();
	switch (cc->c_type)
	{
	case character_type::npc_arch_supervisor:
		result = xml_system::load_dialogue(cur_level, "supervisor_3.xml");
		break;
	case character_type::npc_archaeologist_1:
	case character_type::npc_archaeologist_2:
	case character_type::npc_archaeologist_3:
	case character_type::npc_archaeologist_4:
		result = director::get_secondary_dialogue(target, cc, "archaeologist");
		break;
	}
	return result;
}

xml_system::Dialogue director::juji_start_dialogues(Entity* target)
{
	xml_system::Dialogue result;
	ICharacter* cc = static_cast<ICharacter*>(target->get_component(Component::ComponentType::Character));
	if (!cc)
		return xml_system::Dialogue();
	switch (cc->c_type)
	{
	case character_type::h_jido:
		result = xml_system::load_dialogue(cur_level, "jido_1_1.xml");
		break;
	case character_type::h_josi:
		result = xml_system::load_dialogue(cur_level, "josi_1.xml");
		break;
	case character_type::juji_villager_1:
	case character_type::juji_villager_2:
	case character_type::juji_villager_3:
	case character_type::juji_villager_4:
		result = get_secondary_dialogue(target, cc, "villager");
		break;
	}
	return result;
}

xml_system::Dialogue director::get_secondary_dialogue(Entity* target, ICharacter* cc, std::string char_name)
{
	xml_system::Dialogue result;
	//if player already talked to this person, show same dialogue
	if (!bound.empty())
	{
		for (unsigned int i = 0; i < bound.size(); i++)
		{
			if (bound.at(i).entity->name.compare(target->name) == 0)
			{
				std::cout << "already there at " << i << std::endl;
				return bound.at(i).dialogue;
			}
		}
	}

	std::string path = char_name;
	//if player hasn't talked to character since it's a secondary dialogue, load the appropriate one
	if (secondary_counter > 3)//cap counter at 3
		secondary_counter = 3;
	path = path + "_" + std::to_string(secondary_counter + 1) + ".xml";
	result = xml_system::load_dialogue(cur_level, path);
	secondary_counter++;

	//before this code is run, it's considered the secondary character that player is interacting is is a first type of secondary character in this level
	//so need to replace that with the current target so that it's a correct portrait displayed
	for (unsigned int i = 0; i < result.characters.size(); i++)
	{
		character_type tp = result.characters.at(i);
		if (tp == character_type::npc_archaeologist_1 || tp == character_type::juji_villager_1 || tp == character_type::zakra_villager_1)
			result.characters.at(i) = cc->c_type;
	}
	for (unsigned int i = 0; i < result.lines.size(); i++)
	{
		character_type tp = result.lines.at(i).character;
		if (tp == character_type::npc_archaeologist_1 || tp == character_type::juji_villager_1 || tp == character_type::zakra_villager_1)
			result.lines.at(i).character = cc->c_type;
	}

	bound.push_back({ result, target });
	return result;
}