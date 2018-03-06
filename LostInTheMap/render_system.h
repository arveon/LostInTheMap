#pragma once
#include "Entity.h"
#include "SDL_manager.h"

#include <vector>

class render_system
{
private:
	static std::vector<IDrawable*> background;//either bg or tiles
	static std::vector<IDrawable*> surface;//characters, items, other game objects
	static std::vector<IDrawable*> foreground;
	static std::vector<IDrawable*> ui;

	static std::vector<SDL_Texture*> test;

public:
	static int add_object_to_queue(IDrawable* obj);
	static void flush_queues();
	static void sort_queues();
	static void render_queues();
	static bool remove_from_queue(int id, IDrawable::layers layer);


	static void report_not_null_textures()
	{
		int notnull = 0;
		int totalsize = 0;
		for (unsigned int i = 0; i < test.size(); i++)
		{
			totalsize += sizeof(test.at(i));
			if (test.at(i) != nullptr)
			{
				notnull++;
			}

			
		}

		std::cout << "Total: " << test.size() << ". Not removed: " << notnull << ". Size: " << totalsize << std::endl;
	}
	render_system();
	~render_system();
};

