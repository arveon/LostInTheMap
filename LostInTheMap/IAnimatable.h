#pragma once
#include <SDL.h>

class IAnimatable
{
	SDL_Texture* spritesheet;
	int cur_x_sprite, cur_y_sprite;
	int time_elapsed;
	int total_sprite_required_time = 100;

};