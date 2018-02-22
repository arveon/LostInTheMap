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
	static Uint8* keyboard_state;

	bool initialised;

	SDL_Window* game_window;
	

	static std::vector<callback> mouse_down_callbacks;
	static std::vector<callback> window_close_callbacks;
	static std::vector<HardInputEventType> events;
public:
	static SDL_Renderer * renderer;
	SDL_manager();
	~SDL_manager();

	void initialise(std::string title, int width, int height, bool fullscreen);
	static bool change_resolution(int w_res, int h_res, bool fullscr);

	static int register_mouse_down_listener(callback response) { mouse_down_callbacks.push_back(response); return (static_cast<int>(mouse_down_callbacks.size()) - 1); }
	static bool remove_mouse_down_listener(int listener_id) { if (listener_id < (static_cast<int>(mouse_down_callbacks.size()) - 1)) { mouse_down_callbacks.erase(mouse_down_callbacks.begin() + listener_id); return true; } else return false; };

	static int register_window_close_listener(callback response) { window_close_callbacks.push_back(response); return (static_cast<int>(window_close_callbacks.size()) - 1); }
	static bool remove_window_close_listener(int listener_id) { if (listener_id < (static_cast<int>(window_close_callbacks.size()) - 1)) { window_close_callbacks.erase(window_close_callbacks.begin() + listener_id); return true; } else return false; };

	static void update_input();
	static void trigger_input_listeners();

};

