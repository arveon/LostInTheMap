#pragma once
#include <SDL.h>
#include <vector>
#include <iostream>

class lee_pathfinder
{
public:
	struct pathfinding_tile
	{
		bool is_traversible;
		bool checked;
		std::vector<pathfinding_tile*> neighbours;
	};
	int width;
	int height;

	pathfinding_tile* origin;
	pathfinding_tile* destination;

	static pathfinding_tile*** map;

public:
	static void init_pathfinder(int** map, int width, int height);
	void set_destination(SDL_Point destination) { if (destination.x > 0 && destination.x < width && destination.y > 0 && destination.y < height) this->destination = map[destination.y][destination.x]; };

	lee_pathfinder();
	~lee_pathfinder();
};

