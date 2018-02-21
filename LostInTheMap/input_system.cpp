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
		break;
	case HardInputEventType::left_mouse_up:
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
	return false;
}

input_system::input_system()
{
}


input_system::~input_system()
{
}
