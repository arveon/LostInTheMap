#pragma once
#include <vector>

struct tile
{
	int x, y;
};

class IMovable
{
public:
	float speed;
	bool allowed_movement;
	std::vector<tile> path;

};