#pragma once
#include "SDL_manager.h"
#include "EventTypes.h"

//class will manage all user input
class input_system
{
public:
	struct Mouse
	{
		int x;
		int y;
	};

	static Mouse mouse;

	static int register_event_callback(HardInputEventType, SDL_manager::callback);
	static bool remove_event_callback(HardInputEventType, int);

	static void update_input();

	input_system();
	~input_system();
};

