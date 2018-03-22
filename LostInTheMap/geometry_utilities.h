#pragma once
#include <SDL.h>
class geometry_utilities
{
public:
	static bool is_point_in_rect(int x, int y, SDL_Rect rect) { SDL_Point a{ x,y }; return SDL_PointInRect(&a, &rect); }
	geometry_utilities();
	~geometry_utilities();
};

