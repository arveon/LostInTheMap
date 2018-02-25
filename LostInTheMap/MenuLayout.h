#pragma once
#include <string>
#include <SDL.h>
#include <vector>

struct Button
{
	std::string name;
	SDL_Point position;
};

struct MenuLayout
{
	std::vector<Button> buttons;
	std::string background_path;
};