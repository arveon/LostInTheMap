#include "Game.h"

bool Game::running;
void Game::init()
{
	xml_system::WindowConfig cfg = xml_system::load_config_file();
	sdl_manager.initialise(cfg.title, cfg.width, cfg.height, cfg.fullscreen);
	input_system::register_event_callback(HardInputEventType::window_close, &Game::window_close);
	Game::running = true;
}

void Game::finish()
{

}

void Game::game_loop()
{
	while (Game::running)
	{
		input_system::update_input();
		
	}
}

Game::Game()
{

}



Game::~Game()
{
}
