#include "Game.h"
#include <iostream>

int main(int argc, char* argv[])
{
	Game game;
	game.init();
	game.game_loop();
	game.finish();

	return 0;
}