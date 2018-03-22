#pragma once
#include <SDL.h>
#include <vector>
#include <iostream>

class lee_pathfinder
{
private:
	SDL_Point origin;
	SDL_Point destination;//tile coordinates (in tile numbers, not in world space)
public:
	struct pathfinding_tile
	{
		bool is_traversible;
		bool checked;
		std::vector<pathfinding_tile*> neighbours;
	};
	int width;
	int height;	

	static pathfinding_tile*** map;

public:
	static void init_pathfinder(int** map, int width, int height);
	void set_destination(SDL_Point destination) { if (destination.x > 0 && destination.x < width && destination.y > 0 && destination.y < height) this->destination = destination; };
	std::vector<SDL_Point> get_path_to(SDL_Point destination) { return std::vector<SDL_Point>(); }
	std::vector<SDL_Point> get_path() { return std::vector<SDL_Point>(); }

	lee_pathfinder();
	~lee_pathfinder();
};

