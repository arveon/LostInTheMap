#pragma once
#include <assert.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "SDL_manager.h"

#include "CharacterTypes.h"
#include "ObjectTypes.h"

#include "NameToTypeConversion.h"


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
private:
	static std::vector<SDL_Texture*> terrain_textures;
	static std::map<std::string, SDL_Texture*> object_textures;
	static std::map<character_type, SDL_Texture*> character_textures;
public:
	typedef struct
	{
		SDL_Texture* texture;
		character_type ch_type;
	}CharacterPortrait;

	static SDL_Renderer * renderer;

	static float tile_scaling;

	asset_controller();
	~asset_controller();

	static SDL_Texture* load_texture(const char* path);
	static void destroy_texture(SDL_Texture*);

	static SDL_Texture* get_character_spritesheet(character_type ch);

	static SDL_Rect get_texture_size(SDL_Texture* texture);
	static void set_texture_alpha(SDL_Texture* texture, int alpha);

	static SDL_Texture* create_ui_text_button_spritesheet(std::string text, UI_text_type type);
	static SDL_Texture* get_sprite_from_spritesheet(SDL_Texture* spritesheet, SDL_Rect src_rect);
	static SDL_Texture* get_texture_from_text(std::string text, UI_text_type type, int max_width = 0);

	static void load_terrain_textures(std::string path, int tilewidth, int tileheight);
	static SDL_Texture* get_terrain_texture(unsigned int id) { if (id < terrain_textures.size()) { return terrain_textures.at(id); } else { return nullptr; } }
	static void clear_stored_textures();

	static SDL_Texture* get_object_texture(std::string obj_name);

	static std::vector<CharacterPortrait> get_characters_portraits(std::vector<character_type> dialogue_participants);
};

