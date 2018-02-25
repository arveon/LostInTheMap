#include "MainMenuSystem.h"

MainMenuSystem::func_rem MainMenuSystem::dereg_listener;
int MainMenuSystem::mouse_down_listener_id;
int MainMenuSystem::mouse_up_listener_id;

void MainMenuSystem::init(Space & space, MenuLayout layout, func_reg lmb_down_reg, func_reg lmb_up_reg, func_rem deregister_callback)
{
	dereg_listener = deregister_callback;
	mouse_down_listener_id = lmb_down_reg(&mouse_down_listener);
	mouse_up_listener_id = lmb_up_reg(&mouse_up_listener);


	init_space(space, layout);
}

void MainMenuSystem::init_space(Space & space, MenuLayout layout)
{

	space.initialised = true;
}

void MainMenuSystem::update_space(Space & space, int dt)
{
}

void MainMenuSystem::destroy_space(Space & space)
{
	space.objects.clear();
	space.initialised = false;

	dereg_listener(HardInputEventType::left_mouse_down, mouse_down_listener_id);
	dereg_listener(HardInputEventType::left_mouse_down, mouse_down_listener_id);
}

MainMenuSystem::MainMenuSystem()
{
}


MainMenuSystem::~MainMenuSystem()
{
}
