#include "input_system.h"

int input_system::register_event_callback(HardInputEventType type, SDL_manager::callback callback)
{
	int id = 0;
	switch (type)
	{
	case HardInputEventType::window_close:
		id = SDL_manager::register_window_close_listener(callback);
		std::cout << "Listener registered at id: " << id << std::endl;
		break;
	case HardInputEventType::left_mouse_down:
		id = SDL_manager::register_mouse_down_listener(callback);
		std::cout << "Mouse down listener registered at id: " << id << std::endl;
		break;
	case HardInputEventType::left_mouse_up:
		id = SDL_manager::register_mouse_up_listener(callback);
		std::cout << "Mouse up listener registered at id: " << id << std::endl;
		break;
	case HardInputEventType::right_mouse_down:
		break;
	case HardInputEventType::right_mouse_up:
		break;
	}
	return id;
}

bool input_system::remove_event_callback(HardInputEventType type, int id)
{
	bool result = false;
	switch (type)
	{
	case HardInputEventType::window_close:
		result = SDL_manager::remove_window_close_listener(id);
		break;
	case HardInputEventType::left_mouse_down:
		result = SDL_manager::remove_mouse_down_listener(id);
		break;
	case HardInputEventType::left_mouse_up:
		result = SDL_manager::remove_mouse_up_listener(id);
		break;
	case HardInputEventType::right_mouse_down:
		//SDL_manager::remove_window_close_listener(id);
		break;
	case HardInputEventType::right_mouse_up:
		//SDL_manager::remove_window_close_listener(id);
		break;
	}

	return result;
}

input_system::input_system()
{
}


input_system::~input_system()
{
}
