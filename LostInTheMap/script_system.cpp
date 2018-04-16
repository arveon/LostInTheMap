#include "script_system.h"


Script script_system::cur_script;
int script_system::cur_action;
Space* script_system::game_space;
void(*script_system::start_dialogue_callback)(std::string path);

int script_system::waiting_timer;
int script_system::total_wait_time;

bool script_system::is_script_going()
{
	if (cur_script.actions.size() != 0)
		return (!cur_script.actions.back().finished);
	else
		return false;
}

void script_system::start_script(Script s)
{
	cur_script = s;
	cur_action = 0;
	waiting_timer = 0;
	total_wait_time = 0;

	if (cur_script.blocks_player)
	{
		Entity* player = SpaceSystem::find_entity_by_name(*game_space, "player");
		if (!player)
			return;
		IMoving* mc = static_cast<IMoving*>(player->get_component(Component::ComponentType::Movement));
		mc->movement_allowed = false;
	}

	perform_action();
}

void script_system::action_over(Entity * action_performer)
{
	if (action_performer)
	{//unblock character
		ICharacter* ch = static_cast<ICharacter*>(action_performer->get_component(Component::ComponentType::Character));
		IMoving* mc = static_cast<IMoving*>(action_performer->get_component(Component::ComponentType::Movement));
		ch->controlled_by_script = false;
		mc->movement_allowed = true;
	}


	cur_script.actions.at(cur_action).finished = true;
	cur_action++;
	if (cur_action < (int)cur_script.actions.size())
	{//if more actions are there
		perform_action();
	}		
	else
	{//if no more actions
		//unblock player
		Entity* player = SpaceSystem::find_entity_by_name(*game_space, "player");
		if (!player)
			return;
		IMoving* mc = static_cast<IMoving*>(player->get_component(Component::ComponentType::Movement));
		mc->movement_allowed = true;
	}
}

void script_system::update(int dt)
{
	if (cur_script.actions.at(cur_action).type == action_type::wait)
	{
		waiting_timer += dt;
		if (waiting_timer >= total_wait_time)
		{
			waiting_timer = 0;
			action_over(cur_script.actions.at(cur_action).target);
		}
	}


}

void script_system::reset()
{
	cur_script.actions.clear();
	waiting_timer = 0;
}

void script_system::perform_action()
{
	Action* to_perform = &cur_script.actions.at(cur_action);

	switch (to_perform->type)
	{
	case action_type::movement:
	{
		Entity * to_move = character_system::get_character(to_perform->target_type);
		if (!to_move)
			action_over(nullptr);
		to_perform->target = to_move;
		ICharacter* ch = static_cast<ICharacter*>(to_move->get_component(Component::ComponentType::Character));
		if (!ch)
			action_over(nullptr);
		ch->controlled_by_script = true;
		ch->script_done_callback = &script_system::action_over;

		character_system::set_final_destination_ids(SpaceSystem::get_terrain(*game_space), to_move, to_perform->movement_dest, *game_space);
		break;
	}
	case action_type::dialogue:
	{
		start_dialogue_callback(to_perform->dialogue_path);
		break;
	}
	case action_type::wait:
	{
		Entity * to_stop = character_system::get_character(to_perform->target_type);
		if (!to_stop)
			action_over(nullptr);

		to_perform->target = to_stop;
		IMoving* mc = static_cast<IMoving*>(to_stop->get_component(Component::ComponentType::Movement));
		if(!mc)
			action_over(nullptr);

		mc->movement_allowed = false;
		total_wait_time = to_perform->time;
		waiting_timer = 0;

		break;
	}
	case action_type::change_camera_target:
	{
		Entity * target = character_system::get_character(to_perform->target_type);
		if (!target)
			action_over(nullptr);

		to_perform->target = target;
		camera_system::set_camera_target(target, false, &script_system::action_over);
		
		

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
