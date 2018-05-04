#pragma once
#include <assert.h>
#include <string>
#include <iostream>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "EventTypes.h"

//class will manage basic interaction with the SDL library
class SDL_manager
{
public:
	typedef void(*callback)(void);//pointer to function with no args and return

private:
	typedef struct 
	{
		int x, y;
		bool lmb_down;
		bool rmb_down;
	}mouse;

	static mouse mouse_state;
	//static SDL_Keysym keyboard_state;

	static bool initialised;

	static SDL_Window* game_window;

	static std::vector<callback> mouse_down_callbacks;
	static std::vector<callback> mouse_up_callbacks;
	static std::vector<callback> r_down_callbacks;
	static std::vector<callback> f1_down_callbacks;
	static std::vector<callback> f2_down_callbacks;
	static std::vector<callback> window_close_callbacks;
	static std::vector<HardInputEventType> events;
public:
	typedef struct
	{
		SDL_Texture* tex;
		SDL_Rect draw_rect;
	}Tile;
	
	static SDL_Renderer * renderer;
	
	SDL_manager();
	~SDL_manager();

	void initialise(std::string title, int width, int height, bool fullscreen);
	static bool change_resolution(int w_res, int h_res, bool fullscr);

	static int register_mouse_down_listener(callback response) { mouse_down_callbacks.push_back(response); return (static_cast<int>(mouse_down_callbacks.size()) - 1); }
	static bool remove_mouse_down_listener(int listener_id) 
	{
		if (listener_id <= (static_cast<int>(mouse_down_callbacks.size()) - 1)) 
		{ 
			mouse_down_callbacks.erase(mouse_down_callbacks.begin() + listener_id); return true; 
		} 
		else 
			return false; 
	};

	static int register_mouse_up_listener(callback response) { mouse_up_callbacks.push_back(response); return (static_cast<int>(mouse_up_callbacks.size()) - 1); }
	static bool remove_mouse_up_listener(int listener_id) 
	{ 
		if (listener_id <= (static_cast<int>(mouse_up_callbacks.size()) - 1)) 
		{ 
			mouse_up_callbacks.erase(mouse_up_callbacks.begin() + listener_id); return true; 
		} else 
			return false; 
	};

	static int register_window_close_listener(callback response) { window_close_callbacks.push_back(response); return (static_cast<int>(window_close_callbacks.size()) - 1); }
	static bool remove_window_close_listener(int listener_id) 
	{ if (listener_id <= (static_cast<int>(window_close_callbacks.size()) - 1)) 
		{ 
			window_close_callbacks.erase(window_close_callbacks.begin() + listener_id); return true; 
		} else 
			return false; 
	};

	static int register_r_down_listener(callback response) { r_down_callbacks.push_back(response); return (static_cast<int>(r_down_callbacks.size()) - 1); }
	static bool remove_r_down_listener(int listener_id)
	{
		if (listener_id <= (static_cast<int>(r_down_callbacks.size()) - 1))
		{
			r_down_callbacks.erase(r_down_callbacks.begin() + listener_id); return true;
		}
		else
			return false;
	};

	static int register_f1_down_listener(callback response) { f1_down_callbacks.push_back(response); return (static_cast<int>(f1_down_callbacks.size()) - 1); }
	static bool remove_f1_down_listener(int listener_id)
	{
		if (listener_id <= (static_cast<int>(f1_down_callbacks.size()) - 1))
		{
			f1_down_callbacks.erase(f1_down_callbacks.begin() + listener_id); return true;
		}
		else
			return false;
	};

	static int register_f2_down_listener(callback response) { f2_down_callbacks.push_back(response); return (static_cast<int>(f2_down_callbacks.size()) - 1); }
	static bool remove_f2_down_listener(int listener_id)
	{
		if (listener_id <= (static_cast<int>(f2_down_callbacks.size()) - 1))
		{
			f2_down_callbacks.erase(f2_down_callbacks.begin() + listener_id); return true;
		}
		else
			return false;
	};

	static void update_input();
	static void trigger_input_listeners();

	static void render_sprite(SDL_Texture*, SDL_Rect dest, bool flip_x = false, bool flip_y = false);
	static void render_sprite_src(SDL_Texture*, SDL_Rect src, SDL_Rect dest = { 0,0,-1,-1 });
	static void start_render() 
	{ 
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(SDL_manager::renderer); 
	}
	static void end_render() { SDL_RenderPresent(SDL_manager::renderer); }

	static void get_window_size(int* w, int* h) { SDL_GetWindowSize(SDL_manager::game_window, w, h); }
	static SDL_Texture* get_sprite_from_spritesheet(SDL_Texture* texture, SDL_Rect src_rect);

	static TTF_Font* load_font(const char* path, int size, SDL_Color color);
	static SDL_Texture* load_texture(const char* path);
	
	static SDL_Texture* get_texture_from_text(const char* text, SDL_Color color, TTF_Font* font, int max_width=0);
	static SDL_Texture* get_spritesheet_from_sprites(std::vector<SDL_Texture*> sprites);

	static SDL_Texture* create_terrain_texture(std::vector<SDL_manager::Tile> terrain, int width, int height);

	static SDL_Texture* render_texture_on_texture(SDL_Texture* tex1, SDL_Texture* tex2, int dest_w, int dest_h, SDL_Rect draw_rect);

	static SDL_Texture * create_texture(int w, int h, SDL_Color color);

	static void get_mouse_position(int* x, int* y) { SDL_GetMouseState(x, y); }
};

