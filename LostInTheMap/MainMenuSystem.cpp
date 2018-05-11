#include "MainMenuSystem.h"

MainMenuSystem::func_rem MainMenuSystem::dereg_listener;
int MainMenuSystem::mouse_down_listener_id;
int MainMenuSystem::mouse_up_listener_id;

std::vector<void(*)()> MainMenuSystem::exit_listeners;
std::vector<void(*)()> MainMenuSystem::start_listeners;
std::vector<void(*)()> MainMenuSystem::load_listeners;
std::vector<void(*)()> MainMenuSystem::settings_listeners;
Entity* MainMenuSystem::mouse;

void MainMenuSystem::init(Space & space, MenuLayout layout, func_reg lmb_down_reg, func_reg lmb_up_reg, func_rem deregister_callback)
{
	dereg_listener = deregister_callback;
	mouse_down_listener_id = lmb_down_reg(&MenuSystem::mouse_down_listener);
	mouse_up_listener_id = lmb_up_reg(&MenuSystem::mouse_up_listener);
	MenuSystem::exit = &MainMenuSystem::exit_clicked;
	MenuSystem::start = &MainMenuSystem::start_clicked;
	MenuSystem::mouse = mouse;

	init_space(space, layout);
}

void MainMenuSystem::init_space(Space & space, MenuLayout layout)
{
	camera_system::set_camera_zoom(1.f);
	//initialising background

	MenuSystem::init_menu(layout, space, menu_type::main_menu);
	animator::apply_animation_sprite_changes(space);

#pragma region mouse
	MainMenuSystem::mouse = mouse_system::create_mouse();
	IDrawable* dc = static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable));
	render_system::add_object_to_queue(dc);
	space.objects.push_back(mouse);
#pragma endregion

	add_space_to_render_queue(space);
	
	space.initialised = true;
}

void MainMenuSystem::update_space(Space & space, int dt)
{
	MenuSystem::update_menu_space(space, dt, MainMenuSystem::mouse);
}

void MainMenuSystem::destroy_space(Space & space)
{
	dereg_listener(HardInputEventType::left_mouse_down, mouse_down_listener_id);
	dereg_listener(HardInputEventType::left_mouse_up, mouse_up_listener_id);

	SpaceSystem::destroy_space(space);
}

void MainMenuSystem::exit_clicked()
{
	for (void(*e)() : exit_listeners)
	{
		e();
	}
}

void MainMenuSystem::start_clicked()
{
	for (void(*e)() : start_listeners)
	{
		e();
	}
}


void MainMenuSystem::remove_listeners()
{
	exit_listeners.clear();
	start_listeners.clear();
	load_listeners.clear();
	settings_listeners.clear();
}

MainMenuSystem::MainMenuSystem()
{
}


MainMenuSystem::~MainMenuSystem()
{
}
