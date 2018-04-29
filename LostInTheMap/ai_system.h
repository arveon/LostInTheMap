#pragma once
#include "Entity.h"
#include "ArmyUnit.h"
#include "map_system.h"

class ai_system
{
public:
	static void process_rat_move(Entity* rat, std::vector<army_unit> player_army, ITerrain* tc);
	ai_system();
	~ai_system();
};

