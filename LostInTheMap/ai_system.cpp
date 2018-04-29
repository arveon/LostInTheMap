#include "ai_system.h"



void ai_system::process_rat_move(Entity * rat, std::vector<army_unit> player_army, ITerrain* tc)
{
	//get rats combat unit
	ICombatUnit* cbu = (ICombatUnit*)rat->get_component(Component::ComponentType::CombatUnit);
	cbu->attacking = nullptr;

	//find the weakest unit (least attack)
	army_unit weakest = player_army.at(0);
	for (army_unit u : player_army)
	{
		if (u.max_damage_close < weakest.max_damage_close)
			weakest = u;
	}

	//get weakest units map coordinates
	SDL_Point move = map_system::get_entity_ids(weakest.unit_entity, tc);
	cbu->attacking = weakest.unit_entity;

	//get rats movement component
	IMoving* r_mc = (IMoving*)rat->get_component(Component::ComponentType::Movement);
	r_mc->pathfinder.set_origin(map_system::get_entity_ids(rat, tc));
	r_mc->path = r_mc->pathfinder.get_path_to(move, true);

	if (r_mc->path.size() > 0)//don't go ONTOP of the unit
	{
		r_mc->path.erase(r_mc->path.begin());
	}

	int dif = r_mc->path.size() - cbu->unit_stats.speed;
	if (dif > 0)
	{
		r_mc->path.erase(r_mc->path.begin(), r_mc->path.begin() + dif - 1);
		cbu->attacking = nullptr;
	}



	r_mc->movement_allowed = true;
}

ai_system::ai_system()
{
}


ai_system::~ai_system()
{
}
