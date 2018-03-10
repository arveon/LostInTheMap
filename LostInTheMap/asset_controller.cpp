#include "asset_controller.h"


SDL_Renderer* asset_controller::renderer;
std::vector<SDL_Texture*> asset_controller::terrain_textures;

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
	SDL_Rect rect = {0,0,0,0};
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
	//load appropriate font depending on what text is used for
	SDL_Texture* result = nullptr;

	TTF_Font * font = nullptr;
	std::vector<SDL_Texture*> sprites;

	switch (type)
	{
	case UI_text_type::main_menu_button_main:
		font = SDL_manager::load_font("assets/fonts/Inked_Out.ttf", 16, {0,0,0});
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 191,191,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 168,138,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 100, 70,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 255,255,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 150,150,150 }, font));
		result = SDL_manager::get_spritesheet_from_sprites(sprites);
		break;
	case UI_text_type::main_menu_secondary_button:
		font = SDL_manager::load_font("assets/fonts/Inked_Out.ttf", 12, { 0,0,0 });
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 191,191,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 168,138,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 100, 70,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 255,255,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 150,150,150 }, font));
		result = SDL_manager::get_spritesheet_from_sprites(sprites);
		break;
	case UI_text_type::main_menu_text:
		font = SDL_manager::load_font("assets/fonts/Inked_Out.ttf", 10, { 0,0,0 });
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 191,191,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 168,138,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 100, 70,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 255,255,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 150,150,150 }, font));
		result = SDL_manager::get_spritesheet_from_sprites(sprites);
		break;
	case UI_text_type::game_dialog:
		font = SDL_manager::load_font("assets/fonts/LinLibertine.ttf", 10, { 0,0,0 });
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 191,191,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 168,138,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 100, 70,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 255,255,40 }, font));
		sprites.push_back(SDL_manager::get_texture_from_text(text.c_str(), { 150,150,150 }, font));
		result = SDL_manager::get_spritesheet_from_sprites(sprites);
		break;
	}

	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		destroy_texture(sprites.at(i));
	}

	if(font != nullptr)
		TTF_CloseFont(font);
	return result;
}

SDL_Texture* asset_controller::get_texture_from_text(std::string text, UI_text_type type)
{
	//load appropriate font depending on what text is used for
	SDL_Texture* result = nullptr;

	TTF_Font * font = nullptr;
	std::vector<SDL_Texture*> sprites;

	switch (type)
	{
	case UI_text_type::main_menu_button_main:
		font = SDL_manager::load_font("assets/fonts/Inked_Out.ttf", 16, { 0,0,0 });
		result = SDL_manager::get_texture_from_text(text.c_str(), { 0,0,0 }, font);
		break;
	case UI_text_type::main_menu_secondary_button:
		font = SDL_manager::load_font("assets/fonts/Inked_Out.ttf", 12, { 0,0,0 });
		result = SDL_manager::get_texture_from_text(text.c_str(), { 0,0,0 }, font);
		break;
	case UI_text_type::main_menu_text:
		font = SDL_manager::load_font("assets/fonts/Inked_Out.ttf", 10, { 0,0,0 });
		result = SDL_manager::get_texture_from_text(text.c_str(), { 0,0,0 }, font);
		break;
	case UI_text_type::game_dialog:
		font = SDL_manager::load_font("assets/fonts/LinLibertine.ttf", 10, { 0,0,0 });
		result = SDL_manager::get_texture_from_text(text.c_str(), { 0,0,0 }, font);
		break;
	}

	if (font != nullptr)
		TTF_CloseFont(font);
	return result;
}

void asset_controller::load_terrain_textures(std::string path, int tilewidth)
{
	//TODO: write logic to load all textures from the appropriate spritesheet
	SDL_Texture* tex = load_texture(path.c_str());

	//get number of tiles across and down in tilesheet
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);

	int tileswide = w / tilewidth;
	int tileshigh = h / tilewidth;

	//crop out all individual textures from it
	for (int i = 0; i < tileshigh; i++)
	{
		for (int j = 0; j < tileswide; j++)
		{
			terrain_textures.push_back(get_sprite_from_spritesheet(tex, {tilewidth*j, tilewidth*i, tilewidth, tilewidth}));
		}
	}

	SDL_DestroyTexture(tex);
}

void asset_controller::destroy_terrain_textures()
{
}

SDL_Texture * asset_controller::get_sprite_from_spritesheet(SDL_Texture * spritesheet, SDL_Rect src_rect)
{
	return SDL_manager::get_sprite_from_spritesheet(spritesheet, src_rect);
}

void asset_controller::destroy_texture(SDL_Texture* texture)
{
	SDL_DestroyTexture(texture);
}
