#include "PauseMenuSystem.h"


Space PauseMenuSystem::menu_space;
Entity* PauseMenuSystem::mouse;
void (*PauseMenuSystem::resume)();
void(*PauseMenuSystem::back_to_menu)();

int PauseMenuSystem::down_listener_id;
int PauseMenuSystem::up_listener_id;

void PauseMenuSystem::init_space(Entity* mouse)
{
	MenuLayout layout = xml_system::load_interface_layout("pausemenu");
	MenuSystem::init_menu(layout, menu_space, menu_type::pause_menu);
	SpaceSystem::add_space_to_render_queue(menu_space);
	
	up_listener_id = input_system::register_event_callback(HardInputEventType::left_mouse_up, &MenuSystem::mouse_up_listener);
	down_listener_id = input_system::register_event_callback(HardInputEventType::left_mouse_down, &MenuSystem::mouse_down_listener);

	MenuSystem::resume = &PauseMenuSystem::resume_game;
	MenuSystem::back_to_menu = &PauseMenuSystem::return_to_menu;

	PauseMenuSystem::mouse = mouse;
	MenuSystem::mouse = mouse;
	
	menu_space.initialised = true;
}

void PauseMenuSystem::resume_game()
{
	input_system::remove_event_callback(HardInputEventType::left_mouse_down, down_listener_id);
	input_system::remove_event_callback(HardInputEventType::left_mouse_up, up_listener_id);

	resume_callback();
}

void PauseMenuSystem::return_to_menu()
{
	input_system::remove_event_callback(HardInputEventType::left_mouse_down, down_listener_id);
	input_system::remove_event_callback(HardInputEventType::left_mouse_up, up_listener_id);
	back_to_menu_callback();
}

void PauseMenuSystem::close_menu()
{
	SpaceSystem::destroy_space(menu_space, true);
	input_system::remove_event_callback(HardInputEventType::left_mouse_down, down_listener_id);
	input_system::remove_event_callback(HardInputEventType::left_mouse_up, up_listener_id);
}

void PauseMenuSystem::update_menu(int dt)
{
	MenuSystem::update_menu_space(menu_space, dt, PauseMenuSystem::mouse);
}

PauseMenuSystem::PauseMenuSystem()
{
}


PauseMenuSystem::~PauseMenuSystem()
{
}
