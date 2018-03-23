#include "lee_pathfinder.h"

lee_pathfinder::pathfinding_tile*** lee_pathfinder::map;
int lee_pathfinder::width;
int lee_pathfinder::height;

void lee_pathfinder::init_pathfinder(int ** map, int width, int height)
{
	lee_pathfinder::width = width;
	lee_pathfinder::height = height;
	lee_pathfinder::map = new pathfinding_tile**[height];
	//set up the collision map
	for (int i = 0; i < height; i++)
	{
		lee_pathfinder::map[i] = new pathfinding_tile*[width];
		for(int j = 0; j < width; j++)
		{
			lee_pathfinder::map[i][j] = new pathfinding_tile;
			lee_pathfinder::map[i][j]->pathfinding_value = 0;
			lee_pathfinder::map[i][j]->position = {j, i};
			if (map[i][j] == 1)
				lee_pathfinder::map[i][j]->is_traversible = false;
			else
				lee_pathfinder::map[i][j]->is_traversible = true;

			if (lee_pathfinder::map[i][j]->is_traversible)
				std::cout << "0";
			else
				std::cout << "1";
		}
		std::cout << std::endl;
	}

	//link all of the tiles
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			pathfinding_tile* tile = lee_pathfinder::map[i][j];
			if (i > 0)
				tile->neighbours.push_back(lee_pathfinder::map[i - 1][j]);
			if (j > 0)
				tile->neighbours.push_back(lee_pathfinder::map[i][j - 1]);
			if (i < width - 1)
				tile->neighbours.push_back(lee_pathfinder::map[i + 1][j]);
			if (j < height - 1)
				tile->neighbours.push_back(lee_pathfinder::map[i][j + 1]);
			if (i > 0 && j > 0)
				tile->neighbours.push_back(lee_pathfinder::map[i - 1][j - 1]);
			if (i > 0 && j < height - 1)
				tile->neighbours.push_back(lee_pathfinder::map[i - 1][j + 1]);
			if (i < width - 1 && j > 0)
				tile->neighbours.push_back(lee_pathfinder::map[i + 1][j - 1]);
			if (i < width - 1 && j < height - 1)
				tile->neighbours.push_back(lee_pathfinder::map[i + 1][j + 1]);
		}
}

std::vector<SDL_Point> lee_pathfinder::get_path_to(SDL_Point destination)
{
	std::vector<SDL_Point> path;
	this->destination = destination;
	if (this->origin.x == destination.x && this->origin.y == destination.y)
		return path;
	path = get_path();
	
	return path;
}

std::vector<SDL_Point> lee_pathfinder::get_path()
{
	std::vector<SDL_Point> path;
	
	//mark field from startpoint until endpoint reached

	//get tiles of first generation
	std::vector<pathfinding_tile*> cur_generation = lee_pathfinder::map[origin.y][origin.x]->neighbours;
	bool destination_reached = false;
	int generation_number = 1;
	while (cur_generation.size() != 0 && !destination_reached)
	{
		std::vector<pathfinding_tile*> next_generation;
		for (unsigned int i = 0; i < cur_generation.size(); i++)
		{
			pathfinding_tile* temp = cur_generation.at(i);
			//if tile isn't traversible, don't do anything with it
			if (!temp->is_traversible)
				continue;

			//mark tile with its generation number
			temp->pathfinding_value = generation_number;

			//stop if tile marked is destination
			if (temp->position.x == destination.x && temp->position.y == destination.y)
			{
				destination_reached = true;
				break;
			}
		}

		if (!destination_reached)
		{
			//need the second loop to split marking and assembling next generation (HAS TO BE MARKED FIRST as it could add tiles that will be marked later in THIS generation to NEXT generation)
			for (unsigned int i = 0; i < cur_generation.size(); i++)
			{
				pathfinding_tile* temp = cur_generation.at(i);
				//add all unmarked neighbours of this tile to next generation
				for (unsigned int j = 0; j < temp->neighbours.size(); j++)
				{
					if (!temp->neighbours.at(j)->is_traversible ||temp->neighbours.at(j)->pathfinding_value != 0)
						continue;
					next_generation.push_back(temp->neighbours.at(j));
				}
			}

			cur_generation = next_generation;
			generation_number++;
		}
	}
	
	//display pathfinding values for debug

	for (int i = 0; i < lee_pathfinder::height; i++)
	{
		for (int j = 0; j < lee_pathfinder::width; j++)
		{
			if (i == origin.y && j == origin.x)
			{
				std::cout << "R ";
				continue;
			}
			if(lee_pathfinder::map[i][j]->pathfinding_value > 9)
				std::cout << lee_pathfinder::map[i][j]->pathfinding_value << "";
			else
				std::cout << lee_pathfinder::map[i][j]->pathfinding_value << " ";
		}
		std::cout << std::endl;
	}

	//track back one step at a time and push results to path



	reset_pathfinder();

	return path;
}

void lee_pathfinder::reset_pathfinder()
{
	for (int i = 0; i < lee_pathfinder::height; i++)
		for (int j = 0; j < lee_pathfinder::width; j++)
			lee_pathfinder::map[i][j]->pathfinding_value = 0;
}

lee_pathfinder::lee_pathfinder()
{
}


lee_pathfinder::~lee_pathfinder()
{
}
