#pragma once
#include <assert.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>

#include "SDL_manager.h"

enum UI_text_type
{
	main_menu_button_main,
	main_menu_secondary_button,
	main_menu_list_button,
	main_menu_text,
	game_dialog
};

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
	static void set_texture_alpha(SDL_Texture* texture, int alpha);

	static SDL_Texture* create_ui_text_button_spritesheet(std::string text, UI_text_type type);
	static SDL_Texture* get_sprite_from_spritesheet(SDL_Texture* spritesheet, SDL_Rect src_rect);
	static SDL_Texture* get_texture_from_text(std::string text, UI_text_type type);
};

