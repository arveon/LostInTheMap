#include "Game.h"

bool Game::running;
game_state Game::state;
Space Game::splash_screen_space;
Space Game::main_menu_space;
Space Game::game_space;
Space Game::pause_menu_space;
Space Game::loading_space;
levels Game::cur_level;


void Game::init()
{
	xml_system::WindowConfig cfg = xml_system::load_config_file();

	sdl_manager.initialise(cfg.title, cfg.width, cfg.height, cfg.fullscreen);
	asset_controller::renderer = sdl_manager.renderer;

	input_system::register_event_callback(HardInputEventType::window_close, &Game::window_close_handler);
	input_system::register_event_callback(HardInputEventType::f1_pressed, &Game::dec_level);
	input_system::register_event_callback(HardInputEventType::f2_pressed, &Game::inc_level);
	input_system::register_event_callback(HardInputEventType::r_pressed, &Game::reload_game);
	input_system::register_event_callback(HardInputEventType::esc_pressed, &Game::escape_pressed);
	Game::running = true;
	state = game_state::menu;
	cur_level = levels::pyramid;
	game_time.init();
}

void Game::init_splash()
{
	SplashScreenSystem::init_space(splash_screen_space);
	SplashScreenSystem::add_space_to_render_queue(splash_screen_space);
}

void Game::finish()
{

}

void Game::loading_done_handler()
{
	if (state == game_state::loading)
	{
		level_loading_system::destroy_space(loading_space);
		level_loading_system::remove_listeners();
		render_system::flush_queues();
	}
	state = game_state::game_flow;

}

void Game::splash_elapsed_handler()
{
	if (state == game_state::splash)
	{
		SplashScreenSystem::destroy_space(splash_screen_space);
		render_system::flush_queues();
	}
	state = game_state::menu;
	
}

void Game::start_handler()
{
	if (state == game_state::menu)
	{
		MainMenuSystem::destroy_space(main_menu_space);
		main_menu_space.initialised = false;
		MainMenuSystem::remove_listeners();
		render_system::flush_queues();
	}
	state = game_state::loading;
}

void Game::exit_game_flow()
{
	if (state == game_state::game_flow)
	{
		deregister_event_listener(HardInputEventType::left_mouse_down, game_flow_normal::mouse_down_listener_id);
		deregister_event_listener(HardInputEventType::left_mouse_up, game_flow_normal::mouse_up_listener_id);
		
		game_flow_normal::clear_all_systems(game_space);
		game_flow_normal::destroy_space(game_space);
		render_system::flush_queues();
	}
	state = game_state::menu;
}

void Game::game_loop()
{
	while (Game::running)
	{
		input_system::update_input();
		
		switch (state)
		{
		case game_state::splash:
			if (!splash_screen_space.initialised)
			{
				init_splash();
				SplashScreenSystem::register_splash_elapsed_listener(&splash_elapsed_handler);
			}
			SplashScreenSystem::update_space(splash_screen_space, game_time.get_delta_time());
			break;
		case game_state::menu:
			if (!main_menu_space.initialised)
			{
				MenuLayout layout = xml_system::load_interface_layout("mainmenu");
				MainMenuSystem::init(main_menu_space, layout, &register_mousedown_listener, &register_mouseup_listener, &deregister_event_listener);
				MainMenuSystem::register_exit_listener(&window_close_handler);
				MainMenuSystem::register_start_listener(&start_handler);
			}
			MainMenuSystem::update_space(main_menu_space, game_time.get_delta_time());
			break;
		case game_state::loading:
			if (!loading_space.initialised)
			{
				MenuLayout layout = xml_system::load_interface_layout("loading");
				level_loading_system::init_space(layout,loading_space, cur_level);
				level_loading_system::register_loading_done_listener(&loading_done_handler);
				
			}
			level_loading_system::update_space(loading_space, game_space, game_time.get_delta_time());
			break;
		case game_state::game_flow:
			if (!game_space.initialised)
			{
				game_flow_normal::init(game_space, &Game::change_to_level);
				game_flow_normal::mouse_down_listener_id = input_system::register_event_callback(HardInputEventType::left_mouse_down, game_flow_normal::mouse_down_event);
				game_flow_normal::mouse_up_listener_id = input_system::register_event_callback(HardInputEventType::left_mouse_up, game_flow_normal::mouse_up_event);
				game_flow_normal::return_to_menu = &Game::return_to_menu;
			}
			game_flow_normal::update_space(game_space, game_time.get_delta_time());
			
			break;
		case game_state::confirming_exit:
			
			break;
		}

		game_time.update();
		render_system::sort_queues();
		render_system::render_queues(state);
	}
}
	
void Game::return_to_menu()
{
	exit_game_flow();
	state = game_state::menu;
}

void Game::escape_pressed()
{
	if (game_space.initialised)
		game_flow_normal::trigger_pause();
}

void Game::reload_game()
{
	if (state == game_state::game_flow)
	{
		exit_game_flow();
		state = game_state::loading;
	}
}

void Game::inc_level()
{
	exit_game_flow();
	state = game_state::loading;
	cur_level = ((int)cur_level + 1 > 5) ? (levels)1 : (levels)(cur_level + 1);
}

void Game::dec_level()
{
	exit_game_flow();
	state = game_state::loading;

	cur_level = (cur_level - 1 < 1) ? (levels)5 : (levels)(cur_level - 1);
}

void Game::change_to_level(levels level)
{
	exit_game_flow();
	state = game_state::loading;
	cur_level = level;
}

Game::Game()
{

}



Game::~Game()
{
}
