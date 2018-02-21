#pragma once
#include <SDL.h>

class ICollidable
{
public:
	SDL_Rect collision_rect;
	bool collidable = true;

};