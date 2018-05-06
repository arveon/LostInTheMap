#pragma once
#include "Entity.h"

class overlay_system
{
private:
	enum fade_state
	{
		waiting,
		fading_in,
		fading_out,
		none
	};
	static Entity * fade;
	static int wait_time;
	static int total_time;
	static int elapsed_time;
	static fade_state cur_state;
	static fade_state next_state;

	static void(*fade_done_callback)(Entity*);
public:
	static void init_fade(Entity* fd) { fade = fd; }
	static Entity* get_fade() { return fade; }
	static void set_fade_in(int time, int wait_t, void(*callback)(Entity*)) { cur_state = waiting; next_state = fading_in; wait_time = wait_t; total_time = time; fade_done_callback = callback; }
	static void set_fade_out(int time, void(*callback)(Entity*)) { cur_state = waiting; next_state = fading_out; total_time = time; fade_done_callback = callback; }
	static void update(int dt);
	static bool is_fade_done() { return(cur_state == none); };
	overlay_system();
	~overlay_system();
};

