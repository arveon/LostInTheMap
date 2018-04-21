#include "lee_pathfinder.h"

lee_pathfinder::pathfinding_tile*** lee_pathfinder::map;
int lee_pathfinder::width;
int lee_pathfinder::height;
SDL_Rect lee_pathfinder::camera;

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
			lee_pathfinder::map[i][j]->is_obstructed = false;
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
			if (i < height - 1)
				tile->neighbours.push_back(lee_pathfinder::map[i + 1][j]);
			if (j < width - 1)
				tile->neighbours.push_back(lee_pathfinder::map[i][j + 1]);
			if (i > 0 && j > 0)
			{
				if(lee_pathfinder::map[i-1][j]->is_traversible && lee_pathfinder::map[i][j-1]->is_traversible)
					tile->neighbours.push_back(lee_pathfinder::map[i - 1][j - 1]);
			}
			if (i > 0 && j < width - 1)
			{
				if (lee_pathfinder::map[i - 1][j]->is_traversible && lee_pathfinder::map[i][j+1]->is_traversible)
					tile->neighbours.push_back(lee_pathfinder::map[i - 1][j + 1]);
			}
			if (i < height - 1 && j > 0)
			{
				if (lee_pathfinder::map[i][j - 1]->is_traversible && lee_pathfinder::map[i+1][j]->is_traversible)
					tile->neighbours.push_back(lee_pathfinder::map[i + 1][j - 1]);
			}
			if (i < height - 1 && j < width - 1)
			{
				if (lee_pathfinder::map[i][j + 1]->is_traversible && lee_pathfinder::map[i + 1][j]->is_traversible)
					tile->neighbours.push_back(lee_pathfinder::map[i + 1][j + 1]);
			}
		}
}

std::vector<SDL_Point> lee_pathfinder::get_path_to(SDL_Point destination, bool limit_to_camera_size)
{
	std::vector<SDL_Point> path;
	this->destination = destination;
	if (this->origin.x == destination.x && this->origin.y == destination.y)
		return path;
	path = get_path(limit_to_camera_size);
	
	return path;
}

std::vector<SDL_Point> lee_pathfinder::get_path(bool limit_to_camera_size)
{
	std::vector<SDL_Point> path;
	
	//check if destination is a passable tile
	if (!map[destination.y][destination.x]->is_traversible)
		return path;

	//mark field from startpoint until endpoint reached
	//get tiles of first generation
	std::vector<pathfinding_tile*> cur_generation = lee_pathfinder::map[origin.y][origin.x]->neighbours;
	bool destination_reached = false;
	int generation_number = 1;

	//mark the first generation
	for (unsigned int i = 0; i < cur_generation.size(); i++)
	{
		pathfinding_tile* temp = cur_generation.at(i);
		//if it's players pathfinder, limit pathfinding to camera size
		if (is_player && limit_to_camera_size && (temp->position.x < camera.x || temp->position.x > camera.x + camera.w ||
			temp->position.y < camera.y || temp->position.y > camera.y + camera.h))
			continue;

		//this block makes sure that character won't be able to cut through other objects diagonally (much like what's done with walls when adding neighbours)
		//TOP RIGHT
		if (temp->position.x > lee_pathfinder::map[origin.y][origin.x]->position.x && temp->position.y < lee_pathfinder::map[origin.y][origin.x]->position.y)
			if (lee_pathfinder::map[origin.y - 1][origin.x]->is_obstructed || lee_pathfinder::map[origin.y][origin.x + 1]->is_obstructed)
			{
				i--;
				cur_generation.erase(std::find(cur_generation.begin(), cur_generation.end(), temp));
				continue;
			}
		//TOP LEFT
		if (temp->position.x < lee_pathfinder::map[origin.y][origin.x]->position.x && temp->position.y < lee_pathfinder::map[origin.y][origin.x]->position.y)
			if (lee_pathfinder::map[origin.y - 1][origin.x]->is_obstructed || lee_pathfinder::map[origin.y][origin.x - 1]->is_obstructed)
			{
				i--;
				cur_generation.erase(std::find(cur_generation.begin(), cur_generation.end(), temp));
				continue;
			}

		//BOTTOM LEFT
		if (temp->position.x < lee_pathfinder::map[origin.y][origin.x]->position.x && temp->position.y > lee_pathfinder::map[origin.y][origin.x]->position.y)
			if (lee_pathfinder::map[origin.y + 1][origin.x]->is_obstructed || lee_pathfinder::map[origin.y][origin.x - 1]->is_obstructed)
			{
				i--;
				cur_generation.erase(std::find(cur_generation.begin(), cur_generation.end(), temp));
				continue;
			}

		//BOTTOM RIGHT
		if (temp->position.x > lee_pathfinder::map[origin.y][origin.x]->position.x && temp->position.y > lee_pathfinder::map[origin.y][origin.x]->position.y)
			if (lee_pathfinder::map[origin.y + 1][origin.x]->is_obstructed || lee_pathfinder::map[origin.y][origin.x + 1]->is_obstructed)
			{
				i--;
				cur_generation.erase(std::find(cur_generation.begin(), cur_generation.end(), temp));
				continue;
			}
		
		if (temp->position.x == destination.x && temp->position.y == destination.y)
		{
			temp->pathfinding_value = generation_number;
			destination_reached = true;
			break;
		}

		if (!temp->is_traversible || temp->is_obstructed)
		{
			continue;
		}

		temp->pathfinding_value = generation_number;
	}

	while (cur_generation.size() != 0 && !destination_reached)
	{
		generation_number++;
		std::vector<pathfinding_tile*> next_generation;
		//loop through whole current generation (should all be marked)
		for (unsigned int i = 0; i < cur_generation.size(); i++)
		{
			if (destination_reached)
				break;
			pathfinding_tile* temp = cur_generation.at(i);
			std::vector<pathfinding_tile*> generation = temp->neighbours;

			if (temp->position.x == destination.x && temp->position.y == destination.y)
			{
				temp->pathfinding_value = generation_number;
				destination_reached = true;
				break;
			}

			if (temp->is_obstructed || !temp->is_traversible)
				continue;

			if (temp->pathfinding_value == 0)
			{
				temp->pathfinding_value = generation_number;
				continue;
			}

			//loop through all of the current generation tile's neighbours, mark all of them and add them to next generation
			for (unsigned int j = 0; j < generation.size(); j++)
			{
				pathfinding_tile* temp_next = generation.at(j);

				if (temp_next->position.x == destination.x && temp_next->position.y == destination.y)
				{
					temp_next->pathfinding_value = generation_number;
					destination_reached = true;
					break;
				}

				if (!temp_next->is_traversible)
					continue;

				if (temp_next->pathfinding_value == 0)
				{
					//if it's players pathfinder, limit pathfinding to camera size
					if (is_player && limit_to_camera_size && (temp_next->position.x < camera.x || temp_next->position.x > camera.x + camera.w ||
						temp_next->position.y < camera.y || temp_next->position.y > camera.y + camera.h))
							continue;

					//this block makes sure that character won't be able to cut through other objects diagonally (much like what's done with walls when adding neighbours)
					//TOP RIGHT
					if (temp_next->position.x > temp->position.x && temp_next->position.y < temp->position.y)
						if (lee_pathfinder::map[temp->position.y - 1][temp->position.x]->is_obstructed || 
							lee_pathfinder::map[temp->position.y][temp->position .x + 1]->is_obstructed)
							continue;
					//TOP LEFT
					if (temp_next->position.x < temp->position.x && temp_next->position.y < temp->position.y)
						if (lee_pathfinder::map[temp->position.y - 1][temp->position.x]->is_obstructed || 
							lee_pathfinder::map[temp->position.y][temp->position.x - 1]->is_obstructed)
							continue;

					//BOTTOM LEFT
					if (temp_next->position.x < temp->position.x && temp_next->position.y > temp->position.y)
						if (lee_pathfinder::map[temp->position.y + 1][temp->position.x]->is_obstructed || 
							lee_pathfinder::map[temp->position.y][temp->position.x - 1]->is_obstructed)
							continue;

					//BOTTOM RIGHT
					if (temp_next->position.x > temp->position.x && temp_next->position.y > temp->position.y)
						if (lee_pathfinder::map[temp->position.y + 1][temp->position.x]->is_obstructed || 
							lee_pathfinder::map[temp->position.y][temp->position.x + 1]->is_obstructed)
							continue;

					temp_next->pathfinding_value = generation_number;
					next_generation.push_back(temp_next);
				}

				
				
			}
		}
		cur_generation.clear();
		cur_generation = next_generation;
	}


	
	//if all available tiles marked but destination not reached, means there is no available path
	if (!destination_reached)
	{
		reset_pathfinder();
		return path;
	}

	//print_pathfinding_values();
	path = track_back();
	reset_pathfinder();
	return path;
}

void lee_pathfinder::reset_obstructed()
{
	for (int i = 0; i < lee_pathfinder::height; i++)
		for (int j = 0; j < lee_pathfinder::width; j++)
		{
			lee_pathfinder::map[i][j]->pathfinding_value = 0;
			lee_pathfinder::map[i][j]->is_obstructed = false;
		}
}

void lee_pathfinder::reset_pathfinder()
{
	for (int i = 0; i < lee_pathfinder::height; i++)
		for (int j = 0; j < lee_pathfinder::width; j++)
			lee_pathfinder::map[i][j]->pathfinding_value = 0;
}

void lee_pathfinder::print_pathfinding_values()
{
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
			if (lee_pathfinder::map[i][j]->is_traversible)
			{
				if (lee_pathfinder::map[i][j]->pathfinding_value > 9)
					std::cout << lee_pathfinder::map[i][j]->pathfinding_value << "";
				else
					std::cout << lee_pathfinder::map[i][j]->pathfinding_value << " ";
			}
			else if (!lee_pathfinder::map[i][j]->is_traversible)
				std::cout << "| ";
			else if (lee_pathfinder::map[i][j]->is_obstructed)
				std::cout << "i ";
		}
		std::cout << std::endl;
	}
}

std::vector<SDL_Point> lee_pathfinder::track_back()
{
	std::vector<SDL_Point> path;
	//track back one step at a time and push results to path
	bool start_reached = false;
	pathfinding_tile* cur_tile = lee_pathfinder::map[destination.y][destination.x];
	while (!start_reached)
	{
		//if start reached, break out
		if (cur_tile->position.x == origin.x && cur_tile->position.y == origin.y)
		{
			start_reached = true;
			break;
		}
		path.push_back({ cur_tile->position.x, cur_tile->position.y });

		//loop through all neighbours, find a tile that has pathfinding value cur_tile->pathfinding_value -1
		for (unsigned int i = 0; i < cur_tile->neighbours.size(); i++)
		{
			if (cur_tile->neighbours.at(i)->position.x == origin.x &&
				cur_tile->neighbours.at(i)->position.y == origin.y)
			{
				cur_tile = cur_tile->neighbours.at(i);
				break;
			}
			if (cur_tile->neighbours.at(i)->pathfinding_value == cur_tile->pathfinding_value - 1 && cur_tile->neighbours.at(i)->pathfinding_value > 0 && !cur_tile->neighbours.at(i)->is_obstructed)
			{
				cur_tile = cur_tile->neighbours.at(i);
				break;
			}
		}
	}

	return path;
}

void lee_pathfinder::destroy_pathfinding()
{
	for (int i = 0; i < lee_pathfinder::height; i++)
	{
		for (int j = 0; j < lee_pathfinder::width; j++)
		{
			delete map[i][j];
		}
	}




}

lee_pathfinder::lee_pathfinder()
{
}


lee_pathfinder::~lee_pathfinder()
{
}
