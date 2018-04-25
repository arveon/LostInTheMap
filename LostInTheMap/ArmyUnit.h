#pragma once
#include "CharacterTypes.h"
struct army_unit
{
	character_type type;
	bool is_enemy;
	int quantity;
	int health_of_first; //remaining health of the first unit in stack
	int max_health;
	int min_damage_close;
	int max_damage_close;
	int min_damage_ranged;
	int max_damage_ranged;
	int speed;
	bool ranged_allowed;
};