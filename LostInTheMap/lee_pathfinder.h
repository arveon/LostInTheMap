#pragma once
#include <SDL.h>
#include <vector>
#include <iostream>

class lee_pathfinder
{
private:
	SDL_Point origin;
	SDL_Point destination;//tile coordinates (in tile numbers, not in world space)
	static int width;
	static int height;
public:
	struct pathfinding_tile
	{
		SDL_Point position;
		bool is_traversible;
		bool is_obstructed;
		int pathfinding_value;
		std::vector<pathfinding_tile*> neighbours;
	};
	static pathfinding_tile*** map;

	std::vector<SDL_Point> get_path();
	void reset_pathfinder();
public:
	static void init_pathfinder(int** map, int width, int height);
	void set_destination(SDL_Point destination) 
	{ 
		if (destination.x > 0 && destination.x < width && destination.y > 0 && destination.y < height) 
			this->destination = destination; 
	};
	void set_origin(SDL_Point origin) 
	{ 
		if (origin.x >= 0 && origin.x < lee_pathfinder::width && origin.y >= 0 && origin.y < lee_pathfinder::height) 
			this->origin = origin; 
	};
	std::vector<SDL_Point> get_path_to(SDL_Point destination);
	
	static void reset_obstructed();
	static void set_obstructed(int x, int y) 
	{ 
		if(y < height && y >= 0 && x < width && x >= 0)
			lee_pathfinder::map[y][x]->is_obstructed = true; 
	};

	lee_pathfinder();
	~lee_pathfinder();
};

