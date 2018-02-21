#pragma once

enum unit_type
{
	//TODO: list all the possible units that will be encountered in the game


};

struct unit
{
	unit_type type;
	int quantity;
	int health_of_first; //remaining health of the first unit in stack
};

class IFightable
{
public:
	unit army[6]; //can be up to 6 different units in army
};


