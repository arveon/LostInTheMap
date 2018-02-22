#include "Game.h"

bool Game::running;
void Game::init()
{
	xml_system::WindowConfig cfg = xml_system::load_config_file();

	sdl_manager.initialise(cfg.title, cfg.width, cfg.height, cfg.fullscreen);
	asset_controller::renderer = sdl_manager.renderer;

	input_system::register_event_callback(HardInputEventType::window_close, &Game::window_close);
	Game::running = true;
	state = game_state::splash;
}

void Game::init_splash()
{
	
	SplashScreenSystem::init_space(splash_screen_space);
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
				SplashScreenSystem::init_space(splash_screen_space);
			SplashScreenSystem::update(1);
			break;
		case game_state::main_menu:
		
			break;
		case game_state::pause_menu:
			
			break;
		case game_state::game_flow:
			
			break;
		case game_state::confirming_exit:
			
			break;
		}
	}
}

Game::Game()
{

}



Game::~Game()
{
}
