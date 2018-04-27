#include "asset_controller.h"


SDL_Renderer* asset_controller::renderer;
std::vector<SDL_Texture*> asset_controller::terrain_textures;
std::map<std::string, SDL_Texture*> asset_controller::object_textures;
std::map<character_type, SDL_Texture*> asset_controller::character_textures;

float asset_controller::tile_scaling = 2.f;


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

SDL_Texture* asset_controller::get_texture_from_text(std::string text, UI_text_type type, int max_width)
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
		font = SDL_manager::load_font("assets/fonts/LinLibertine.ttf", 20, { 0,0,0 });
		result = SDL_manager::get_texture_from_text(text.c_str(), { 0,0,0 }, font, max_width);
		break;
	case UI_text_type::game_ui_small:
		font = SDL_manager::load_font("assets/fonts/Chapaza.ttf", 10, { 0,0,0 });
		result = SDL_manager::get_texture_from_text(text.c_str(), { 0,0,0 }, font, max_width);
		break;
	}

	if (font != nullptr)
		TTF_CloseFont(font);
	return result;
}

void asset_controller::load_terrain_textures(std::string path, int tilewidth, int tileheight)
{
	SDL_Texture* tex = load_texture(path.c_str());

	//get number of tiles across and down in tilesheet
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);

	int tileswide = w / tilewidth;
	int tileshigh = h / tileheight;

	//crop out all individual textures from it
	for (int i = 0; i < tileshigh; i++)
	{
		for (int j = 0; j < tileswide; j++)
		{
			terrain_textures.push_back(get_sprite_from_spritesheet(tex, {tilewidth*j, tilewidth*i, tilewidth, tileheight}));
		}
	}

	SDL_DestroyTexture(tex);
}

void asset_controller::clear_stored_textures()
{
	for (SDL_Texture* t : terrain_textures)
	{
		asset_controller::destroy_texture(t);
	}
	terrain_textures.clear();

	for (std::pair<std::string, SDL_Texture*> t : object_textures)
	{
		asset_controller::destroy_texture(t.second);
	}
	object_textures.clear();

	for (std::pair<character_type, SDL_Texture*> pr : character_textures)
	{
		asset_controller::destroy_texture(pr.second);
	}
	character_textures.clear();
}

SDL_Texture* asset_controller::get_object_texture(std::string obj_name)
{
	SDL_Texture* text = object_textures[obj_name];
	if (!object_textures[obj_name])
	{
		std::string path = "assets/graphics/objects/" + obj_name + ".png";
		object_textures[obj_name] = load_texture(path.c_str());
	}
	return object_textures[obj_name];
}

std::vector<asset_controller::CharacterPortrait> asset_controller::get_characters_portraits(std::vector<character_type> dialogue_participants)
{
	std::vector<CharacterPortrait> result;

	for (unsigned int i = 0; i < dialogue_participants.size(); i++)
	{
		CharacterPortrait temp;
		temp.ch_type = dialogue_participants.at(i);
		std::string fullpath = "assets/graphics/characters/portraits/" + NameToTypeConversion::get_character_name_by_type(dialogue_participants.at(i)) + "_portrait.png";
		temp.texture = asset_controller::load_texture(fullpath.c_str());

		result.push_back(temp);
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

SDL_Texture* asset_controller::get_character_spritesheet(character_type ch)
{
	for (std::pair<character_type, SDL_Texture*> pr : character_textures)
	{
		if (pr.first == ch)
			return pr.second;
	}

	std::string path = "assets/graphics/characters/" + NameToTypeConversion::get_character_name_by_type(ch) + ".png";
	SDL_Texture* result = load_texture(path.c_str());

	character_textures[ch] = result;

	return result;
}

SDL_Texture* asset_controller::get_texture_from_two(SDL_Texture* tex, SDL_Texture* tex2, int dest_w, int dest_h, SDL_Rect draw_rect)
{
	return SDL_manager::render_texture_on_texture(tex, tex2, dest_w, dest_h, draw_rect);
}
