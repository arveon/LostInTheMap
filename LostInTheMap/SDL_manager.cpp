#include "SDL_manager.h"

std::vector<SDL_manager::callback> SDL_manager::mouse_down_callbacks;
std::vector<SDL_manager::callback> SDL_manager::mouse_up_callbacks;
std::vector<SDL_manager::callback> SDL_manager::window_close_callbacks;
SDL_manager::mouse SDL_manager::mouse_state;
std::vector<HardInputEventType> SDL_manager::events;
SDL_Renderer* SDL_manager::renderer;

SDL_manager::SDL_manager()
{
}


SDL_manager::~SDL_manager()
{
}

void SDL_manager::initialise(std::string title, int width, int height, bool fullscreen)
{
	assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	if (!fullscreen)
		game_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN/* || SDL_WINDOW_FULLSCREEN*/);
	else
		game_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN || SDL_WINDOW_FULLSCREEN);

	//initialise window and renderer
	assert(game_window);
	renderer = SDL_CreateRenderer(game_window, -1, SDL_RENDERER_ACCELERATED);
	assert(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_ADD);
	//SDL_RenderSetScale(renderer, constants::scaling_horizontal, constants::scaling_vertical);

	//initialise img and mix libraries
	int img_flags = IMG_INIT_PNG;

	assert(IMG_Init(img_flags) & img_flags);
	assert(TTF_Init() != -1);
	assert(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) >= 0);
	initialised = true;

	//hide mouse cursor so the custom one can be drawn instead
	SDL_ShowCursor(0);
}


bool SDL_manager::change_resolution(int w_res, int h_res, bool fullscr)
{
	return false;
}

void SDL_manager::update_input()
{
	//storing mouse clicks and mouse releases in mouse class
	//reading events such as windows X button pressed
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
		{
			events.push_back(HardInputEventType::window_close);
		}

		//storing mouse clicks and mouse releases in mouse class
		if (event.type == SDL_MOUSEBUTTONDOWN)//reading mouse events to determine when the click occurs
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				events.push_back(HardInputEventType::left_mouse_down);
				mouse_state.lmb_down = true;
			}
			else
			{
				events.push_back(HardInputEventType::right_mouse_down);
				mouse_state.rmb_down = true;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				events.push_back(HardInputEventType::left_mouse_up);
				mouse_state.lmb_down = false;
			}
			else
			{
				events.push_back(HardInputEventType::right_mouse_up);
				mouse_state.rmb_down = false;
			}
		}
	}
	//keyboard_state = SDL_GetKeyboardState();
}

void SDL_manager::trigger_input_listeners()
{
	while (events.size() > 0)
	{
		HardInputEventType type = events.at(events.size()-1);
		switch (type)
		{
		case HardInputEventType::window_close:
			for (int i = 0; i < window_close_callbacks.size(); i++)
			{
				callback cb = window_close_callbacks.at(i);
				cb();
			}
			break;
		case HardInputEventType::left_mouse_down:
			for (int i = 0; i < mouse_down_callbacks.size(); i++)
			{
				callback cb = mouse_down_callbacks.at(i);
				cb();
			}
			break;
		case HardInputEventType::left_mouse_up:
			for (int i = 0; i < mouse_up_callbacks.size(); i++)
			{
				callback cb = mouse_up_callbacks.at(i);
				cb();
			}
			break;

		}

		events.pop_back();
	}

}

void SDL_manager::render_sprite(SDL_Texture * texture, SDL_Rect dest)
{
	SDL_RenderCopy(SDL_manager::renderer, texture, nullptr, &dest);
}
