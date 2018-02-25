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
	return SDL_manager::load_texture(path);
}

SDL_Rect asset_controller::get_texture_size(SDL_Texture * texture)
{
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	return rect;
}

//alpha 0 - 255
void asset_controller::set_texture_alpha(SDL_Texture * texture, int alpha)
{
	if (alpha > 255) alpha = 255;
	else if (alpha < 0) alpha = 0;
	SDL_SetTextureAlphaMod(texture, alpha);
}

SDL_Texture * asset_controller::create_ui_text_button_spritesheet(std::string text, UI_text_type type)
{
	//TODO: finish this method for all the cases of type
	//load appropriate font depending on what text is used for
	SDL_Texture* result = nullptr;
	switch (type)
	{
	case UI_text_type::main_menu_button_main:
		
		break;
	case UI_text_type::main_menu_secondary_button:
		break;
	}
	return result;
}

SDL_Texture * asset_controller::get_sprite_from_spritesheet(SDL_Texture * spritesheet, SDL_Rect src_rect)
{
	return SDL_manager::get_sprite_from_spritesheet(spritesheet, src_rect);
}

void asset_controller::destroy_texture(SDL_Texture* texture)
{
	SDL_DestroyTexture(texture);
}
