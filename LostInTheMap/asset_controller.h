#pragma once
#include <assert.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

//class will have functionality of loading/unloading visual and audio assets
class asset_controller
{
public:
	static SDL_Renderer * renderer;
	asset_controller();
	~asset_controller();

	static SDL_Texture* load_texture(const char* path);
	static void destroy_texture(SDL_Texture*);

	static SDL_Rect get_texture_size(SDL_Texture* texture);
};

