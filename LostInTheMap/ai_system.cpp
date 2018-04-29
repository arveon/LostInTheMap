#include "ai_system.h"



void ai_system::process_rat_move(Entity * rat, std::vector<army_unit*> player_army, ITerrain* tc)
{
	//get rats combat unit
	ICombatUnit* cbu = (ICombatUnit*)rat->get_component(Component::ComponentType::CombatUnit);
	//get rats movement component
	IMoving* r_mc = (IMoving*)rat->get_component(Component::ComponentType::Movement);
	cbu->attacking = nullptr;

	//construct priority queue for the possible targets
	std::vector<std::pair<army_unit*, float>> priority_queue;
	SDL_Point cur_unit_ids = map_system::get_entity_ids(rat, tc);
	for (army_unit* u : player_army)
	{
		std::pair<army_unit*, float> unit_priority;
		unit_priority.first = u;
		
		//calculate distance
		SDL_Point player_unit_ids = map_system::get_entity_ids(u->unit_entity, tc);
		SDL_Point dist;
		dist.x = std::abs(cur_unit_ids.x - player_unit_ids.x);
		dist.y = std::abs(cur_unit_ids.y - player_unit_ids.y);
		float full_dist = std::sqrt(std::pow(dist.x,2) + std::pow(dist.y, 2));

		unit_priority.second = (1.f / full_dist) + 1.f/u->max_damage_close;

		int priority_lvl = 0;
		for (int i = 0; i < priority_queue.size(); i++)
		{
			if (unit_priority.second > priority_queue.at(i).second)
			{
				priority_lvl = i;
				break;
			}
			else
				continue;

			
		}
		priority_queue.insert(priority_queue.begin() + priority_lvl, unit_priority);
	}


	bool target_found = false;
	while (!target_found)
	{
		army_unit* potential_target = priority_queue.back().first;

		//measure distance
		//calculate distance
		SDL_Point player_unit_ids = map_system::get_entity_ids(potential_target->unit_entity, tc);
		SDL_Point dist;
		dist.x = std::abs(cur_unit_ids.x - player_unit_ids.x);
		dist.y = std::abs(cur_unit_ids.y - player_unit_ids.y);
		float full_dist = std::sqrt(std::pow(dist.x, 2) + std::pow(dist.y, 2));

		//calculate path
		r_mc->pathfinder.set_origin(cur_unit_ids);
		r_mc->path = r_mc->pathfinder.get_path_to(player_unit_ids, true);

		//if path is 0 but distance is > 0, then remove this target from priority queue and continue
		if (r_mc->path.size() == 0 && full_dist > 0.01f)
		{
			priority_queue.pop_back();
			continue;
		}
		else if (r_mc->path.size() > 0)
		{//if path is more than 0, do the magic
			cbu->attacking = potential_target->unit_entity;
			r_mc->path.erase(r_mc->path.begin());//just to make sure don't go ontop of unit
			int dif = r_mc->path.size() - cbu->unit_stats->speed;
			if (dif > 0)
			{
				r_mc->path.erase(r_mc->path.begin(), r_mc->path.begin() + dif - 1);
				cbu->attacking = nullptr;
			}
			target_found = true;
		}

		

	}



	//while (!target_found)
	//{
	//	//find the weakest unit (least attack)
	//	army_unit* weakest = player_army.at(0);
	//	for (army_unit* u : player_army)
	//	{
	//		//check if already tried reaching it and unreachable
	//		bool is_ignored = false;
	//		for (army_unit* ignored : ignored_units)
	//			if (ignored == u)
	//			{
	//				is_ignored = true;
	//				break;
	//			}
	//		if (is_ignored)
	//		{
	//			continue;
	//		}

	//		
	//		if (u->max_damage_close < weakest->max_damage_close)
	//			weakest = u;
	//	}

	//	//get weakest units map coordinates
	//	SDL_Point move = map_system::get_entity_ids(weakest->unit_entity, tc);
	//	cbu->attacking = weakest->unit_entity;
	//	
	//	r_mc->pathfinder.set_origin(map_system::get_entity_ids(rat, tc));
	//	r_mc->path = r_mc->pathfinder.get_path_to(move, true);

	//	if (r_mc->path.size() > 0)//don't go ONTOP of the unit
	//	{
	//		r_mc->path.erase(r_mc->path.begin());
	//		target_found = true;
	//	}

	

	//}

	r_mc->movement_allowed = true;
}

ai_system::ai_system()
{
}


ai_system::~ai_system()
{
}
