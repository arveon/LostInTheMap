#pragma once
#include "Space.h"

class SpaceSystem
{
public:
	SpaceSystem();
	~SpaceSystem();
	virtual void update_space(int dt, Space space);
	void initialise_space(Space space);
	void destroy_space(Space space);
};

