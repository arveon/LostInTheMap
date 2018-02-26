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

public:
	static void add_object_to_queue(IDrawable* obj);
	static void flush_queues();
	static void sort_queues();
	static void render_queues();

	render_system();
	~render_system();
};
