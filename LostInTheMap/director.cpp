#include "director.h"

director::story_stage director::cur_stage;
int director::secondary_counter;
std::vector<director::DialogBound> director::bound;
std::vector<std::string> director::triggered_scripts;
levels director::cur_level;

xml_system::Dialogue director::get_dialogue(Entity * target)
{
	xml_system::Dialogue result;
	switch (cur_stage)
	{
	case pyramid_entrance:
		ICharacter* cc = static_cast<ICharacter*>(target->get_component(Component::ComponentType::Character));
		if (!cc)
			return xml_system::Dialogue();
		switch (cc->c_type)
		{
		case character_type::npc_arch_supervisor:
			result = xml_system::load_dialogue("Levels/pyramid/dialogs/supervisor_1.xml");
			break;
		case character_type::npc_archaeologist_1:
		case character_type::npc_archaeologist_2:
		case character_type::npc_archaeologist_3:
		case character_type::npc_archaeologist_4:
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

			//if player hasn't talked to character since it's a secondary dialogue, load the appropriate one
			if(secondary_counter == 0)
				result = xml_system::load_dialogue("Levels/pyramid/dialogs/archaeologist_1.xml");
			else if(secondary_counter == 1)
				result = xml_system::load_dialogue("Levels/pyramid/dialogs/archaeologist_2.xml");
			else if(secondary_counter == 2)
				result = xml_system::load_dialogue("Levels/pyramid/dialogs/archaeologist_3.xml");
			else if(secondary_counter >= 3)
				result = xml_system::load_dialogue("Levels/pyramid/dialogs/archaeologist_4.xml");
			secondary_counter++;

			//check all of the loaded characters and make sure proper archaeologist portraits are loaded
			for (unsigned int i = 0; i < result.characters.size(); i++)
			{
				if (result.characters.at(i) == character_type::npc_archaeologist_1)
					result.characters.at(i) = cc->c_type;
			}
			for (unsigned int i = 0; i < result.lines.size(); i++)
			{
				if (result.lines.at(i).character == character_type::npc_archaeologist_1)
					result.lines.at(i).character = cc->c_type;
			}

			bound.push_back({ result, target });
			break;
		}


		break;

	}

	return result;
}

void director::process_interaction(Entity* interaction_target)
{
	if (interaction_target->name.compare("player") == 0)
		return;


	IInteractionSource* interaction_src = static_cast<IInteractionSource*>(interaction_target->get_component(Component::ComponentType::InteractionSource));
	//this block either starts combat or engages dialogue if it's a character (to avoid having to write script that starts a dialogue for every npc)
	ICharacter* target_char = static_cast<ICharacter*>(interaction_target->get_component(Component::ComponentType::Character));
	if (target_char)
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
		else
		{
			std::cout << "combat started" << std::endl;
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
	xml_system::Dialogue d = xml_system::load_dialogue(path);
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
		script_system::start_script(script);
		triggered_scripts.push_back(is->script_attached);
	}
	else
		std::cout << "no is?!" << std::endl;
}

void director::reset_director()
{
	secondary_counter = 0; 
	bound.clear(); 
	triggered_scripts.clear();

	dialogue_system::reset();
	script_system::reset();
}

void director::init_stage(levels level)
{
	director::cur_level = level;
	script_system::set_dialogue_start_callback(director::start_scripted_dialogue);
}

director::director()
{}


director::~director()
{}
