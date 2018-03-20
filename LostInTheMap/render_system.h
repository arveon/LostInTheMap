#pragma once
#include "Entity.h"
#include "SDL_manager.h"
#include "camera_system.h"

#include <vector>

class render_system
{
private:
	static float scaling_w;
	static float scaling_h;
	static std::vector<IDrawable*> background;//bg
	static std::vector<IDrawable*> terrain;
	static std::vector<IDrawable*> surface;//characters, items, other game objects
	static std::vector<IDrawable*> foreground;
	static std::vector<IDrawable*> ui;

public:
	static int add_object_to_queue(IDrawable* obj);
	static void flush_queues();
	static void sort_queues();
	static void render_queues();
	static bool remove_from_queue(int id, IDrawable::layers layer);
	render_system();
	~render_system();
};

