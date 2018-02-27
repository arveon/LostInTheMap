#pragma once
#include <SDL.h>
class sdl_utils
{
public:
	static bool is_point_in_rect(SDL_Point point, SDL_Rect rect) { return SDL_PointInRect(&point, &rect); }
	sdl_utils();
	~sdl_utils();
};

