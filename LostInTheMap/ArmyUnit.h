#pragma once
#include "CharacterTypes.h"
struct army_unit
{
	character_type type;
	int quantity;
	int health_of_first; //remaining health of the first unit in stack
	int max_health;
	int min_damage;
	int max_damage;
	int speed;
};