#pragma once
#include <SDL.h>
#include <string>

class IDescriptable
{
public:
	SDL_Texture * box_background;
	SDL_Texture* rendered_text;
	std::string text;
	SDL_Rect box_draw_rect;

};