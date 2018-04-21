#include "script_system.h"


Script script_system::cur_script;
int script_system::cur_action;
Space* script_system::game_space;
void(*script_system::start_dialogue_callback)(std::string path);
void(*script_system::state_change_callback)(std::string path);
void(*script_system::combat_start_callback)(levels level, Space&, IFightable*);
void(*script_system::level_change_callback)(levels level);


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
		if (!ch || !to_move->is_active)
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
		total_wait_time = to_perform->time;
		waiting_timer = 0;

		//if character specified, stop that character from moving
		Entity * to_stop = character_system::get_character(to_perform->target_type);
		if (to_stop)
		{
			

			to_perform->target = to_stop;
			IMoving* mc = static_cast<IMoving*>(to_stop->get_component(Component::ComponentType::Movement));
			if (!mc)
				action_over(nullptr);
			mc->movement_allowed = false;
		}

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
	case action_type::move_camera_to_tile:
	{
		Entity * target = SpaceSystem::get_tile_at_ids(*game_space, to_perform->movement_dest.x, to_perform->movement_dest.y);
		if (!target)
			action_over(nullptr);

		camera_system::set_camera_target(target, false, &script_system::action_over);
		break;
	}
	case action_type::start_combat:
	{
		Entity* player = SpaceSystem::find_entity_by_name(*game_space, "player");
		IInteractionSource* src = static_cast<IInteractionSource*>(player->get_component(Component::ComponentType::InteractionSource));
		Entity * target = src->interaction_target;
		if (!target)
			action_over(nullptr);

		IFightable* fc = static_cast<IFightable*>(target->get_component(Component::ComponentType::Fighting));
		combat_start_callback(director::cur_level, *game_space, fc);
		break;
	}
	case action_type::set_story_state:
	{
		script_system::state_change_callback(to_perform->utility);
		action_over(nullptr);
		break;
	}
	case action_type::object_disappear:
	{
		if (to_perform->utility.compare("target") == 0)
		{
			Entity* pl = SpaceSystem::find_entity_by_name(*game_space, "player");
			IInteractionSource* src = static_cast<IInteractionSource*>(pl->get_component(Component::ComponentType::InteractionSource));
			src->interaction_target->deactivate();
		}
		else
		{
			object_types t = xml_system::get_object_type_by_name(to_perform->utility);
			Entity* ent = SpaceSystem::find_object_of_type(*game_space, t);
			ent->deactivate();
		}
		action_over(nullptr);
		break;
	}
	case action_type::character_disappear:
	{
		Entity* character = character_system::get_character(to_perform->target_type);
		character->deactivate();
		action_over(nullptr);
		break;
	}
	case action_type::level_switch:
	{
		levels target_level = xml_system::get_level_type_from_name(to_perform->utility);
		level_change_callback(target_level);
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
