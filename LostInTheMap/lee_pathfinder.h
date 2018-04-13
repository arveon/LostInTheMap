#pragma once
#include <SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>

class lee_pathfinder
{
private:
	SDL_Point origin;
	SDL_Point destination;//tile coordinates (in tile numbers, not in world space)
	static int width;
	static int height;
	static SDL_Rect camera;
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

	std::vector<SDL_Point> get_path(bool limit_to_map_size);
	std::vector<SDL_Point> get_path_to(SDL_Point destination, bool limit_to_map_size);
	void reset_pathfinder();
	void print_pathfinding_values();
	std::vector<SDL_Point> track_back();

	bool is_player = false;

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
	
	static void reset_obstructed();
	static void set_obstructed(int x, int y) 
	{ 
		if(y < height && y >= 0 && x < width && x >= 0)
			lee_pathfinder::map[y][x]->is_obstructed = true; 
	};

	static void set_camera_position(int x, int y) { camera.x = x; camera.y = y; }
	static void set_camera_dimensions(int w, int h) { camera.w = w; camera.h = h; }

	static void destroy_pathfinding();

	lee_pathfinder();
	~lee_pathfinder();
};

