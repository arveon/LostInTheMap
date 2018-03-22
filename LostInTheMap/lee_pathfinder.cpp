#include "lee_pathfinder.h"

lee_pathfinder::pathfinding_tile*** lee_pathfinder::map;

void lee_pathfinder::init_pathfinder(int ** map, int width, int height)
{
	lee_pathfinder::map = new pathfinding_tile**[height];
	for (int i = 0; i < height; i++)
	{
		lee_pathfinder::map[i] = new pathfinding_tile*[width];
		for(int j = 0; j < width; j++)
		{
			lee_pathfinder::map[i][j] = new pathfinding_tile;
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

}

lee_pathfinder::lee_pathfinder()
{
}


lee_pathfinder::~lee_pathfinder()
{
}
