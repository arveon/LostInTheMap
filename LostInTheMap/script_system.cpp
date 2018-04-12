#include "script_system.h"


Script script_system::cur_script;
int script_system::cur_action;
Space* script_system::game_space;

void script_system::start_script(Script s)
{
	cur_script = s;
	cur_action = 0;
	perform_action();
}

void script_system::action_over(Entity * action_performer)
{
	if (action_performer)
	{
		ICharacter* ch = static_cast<ICharacter*>(action_performer->get_component(Component::ComponentType::Character));
		ch->controlled_by_script = false;
	}


	cur_script.actions.at(cur_action).finished = true;
	cur_action++;
	if (cur_action < cur_script.actions.size())
	{
		//perform_action();
	}		
}

void script_system::perform_action()
{
	Action to_perform = cur_script.actions.at(cur_action);

	switch (to_perform.type)
	{
	case action_type::movement:
	{
		Entity * to_move = character_system::get_character(to_perform.target_type);
		if (!to_move)
			return;

		ICharacter* ch = static_cast<ICharacter*>(to_move->get_component(Component::ComponentType::Character));
		if (!ch)
			return;
		ch->controlled_by_script = true;
		ch->script_done_callback = &script_system::action_over;

		character_system::set_final_destination_ids(SpaceSystem::get_terrain(*game_space), to_move, to_perform.movement_dest, *game_space);
		break;
	}
	case action_type::dialogue:
	{
		action_over(nullptr);
		break;
	}
	}



}




script_system::script_system()
{
}


script_system::~script_system()
{
}
