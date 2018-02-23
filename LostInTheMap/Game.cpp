#include "Game.h"

bool Game::running;
game_state Game::state;

void Game::init()
{
	xml_system::WindowConfig cfg = xml_system::load_config_file();

	sdl_manager.initialise(cfg.title, cfg.width, cfg.height, cfg.fullscreen);
	asset_controller::renderer = sdl_manager.renderer;

	input_system::register_event_callback(HardInputEventType::window_close, &Game::window_close_handler);
	Game::running = true;
	state = game_state::splash;
	time.init();
}

void Game::init_splash()
{
	SplashScreenSystem::init_space(splash_screen_space);
	SplashScreenSystem::add_space_to_render_queue(splash_screen_space);
}

void Game::finish()
{

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
			SplashScreenSystem::update_space(splash_screen_space, time.get_delta_time());
			
			break;
		case game_state::main_menu:
			std::cout << "main menu" << std::endl;
			break;
		case game_state::pause_menu:
			
			break;
		case game_state::game_flow:
			
			break;
		case game_state::confirming_exit:
			
			break;
		}

		time.update();
		render_system::render_queues();
	}
}

Game::Game()
{

}



Game::~Game()
{
}
