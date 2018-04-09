#include "director.h"

director::story_stage director::cur_stage;
int director::secondary_counter;
std::vector<director::DialogBound> director::bound;

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
		case character_type::h_zaji:
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

			bound.push_back({ result, target });
			break;
		}


		break;

	}

	return result;
}

void director::process_interaction(Entity* interaction_target)
{
	ICharacter* target_char = static_cast<ICharacter*>(interaction_target->get_component(Component::ComponentType::Character));
	if (target_char)
	{
		if (target_char->is_friendly)
		{
			xml_system::Dialogue dial = director::get_dialogue(interaction_target);
			dialogue_system::start_dialogue(dial);
		}
		else
		{
			std::cout << "combat started" << std::endl;
		}
	}


}

director::director()
{
}


director::~director()
{
}
