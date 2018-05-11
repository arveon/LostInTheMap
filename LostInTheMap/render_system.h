#pragma once
#include "Entity.h"
#include "SDL_manager.h"
#include "camera_system.h"
#include "GameStates.h"

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
	static std::vector<IDrawable*> world_ui;
	static SDL_Texture* terrain_sprite;
	static bool terrain_prepared;
	static IDrawable* mouse;

public:
	static int add_object_to_queue(IDrawable* obj);
	static void flush_queues();
	static void sort_queues();
	static void render_queues(game_state state);
	static void set_terrain_texture(SDL_Texture * tex);
	
	static void prepare_terrain(int map_width, int map_height);
	static void unprepare_terrain();
	//width and height in pixels
	static bool remove_from_queue(IDrawable*);//remove the given component from queue
	render_system();
	~render_system();
};

