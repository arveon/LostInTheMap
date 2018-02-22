#include "asset_controller.h"


SDL_Renderer* asset_controller::renderer;

asset_controller::asset_controller()
{
}


asset_controller::~asset_controller()
{
}

SDL_Texture * asset_controller::load_texture(const char * path)
{
	SDL_Texture* temp = IMG_LoadTexture(renderer, path);
	std::cout << IMG_GetError() << " for image " << path;
	assert(temp);
	return temp;
}

SDL_Rect asset_controller::get_texture_size(SDL_Texture * texture)
{
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	return rect;
}

void asset_controller::destroy_texture(SDL_Texture* texture)
{
	SDL_DestroyTexture(texture);
}
