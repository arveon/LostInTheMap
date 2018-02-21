#pragma once
#include "SDL_manager.h"
#include "EventTypes.h"

//class will manage all user input
class input_system
{
private:
	
public:
	static int register_event_callback(HardInputEventType, SDL_manager::callback);
	static bool remove_event_callback(HardInputEventType, int);

	static void update_input() { SDL_manager::update_input(); SDL_manager::trigger_input_listeners(); };

	input_system();
	~input_system();
};

