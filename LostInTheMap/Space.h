#pragma once
#include "Entity.h"
#include <vector>

class Space
{
public:
	bool initialised = false;
	std::vector<Entity*> objects;

	~Space() {};
};

