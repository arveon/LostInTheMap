#include "asset_controller.h"


SDL_Renderer* asset_controller::renderer;
std::vector<SDL_Texture*> asset_controller::terrain_textures;
std::map<std::string, SDL_Texture*> asset_controller::object_textures;

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
	}

	if (font != nullptr)
		TTF_CloseFont(font);
	return result;
}

void asset_controller::load_terrain_textures(std::string path, int tilewidth)
{
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
	for (unsigned int i = 0; i < terrain_textures.size(); i++)
	{
		asset_controller::destroy_texture(terrain_textures.at(i));
	}
	terrain_textures.clear();
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
		switch (dialogue_participants.at(i))
		{
		case h_giovanni:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/giovanni_portrait.png");
			break;
		case npc_arch_supervisor://LEVEL 1
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/supervisor_portrait.png");
			break;
		case npc_archaeologist_1:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/archaeologist_1_portrait.png");
			break;
		case npc_archaeologist_2:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/archaeologist_2_portrait.png");
			break;
		case npc_archaeologist_3:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/archaeologist_3_portrait.png");
			break;
		case npc_archaeologist_4:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/archaeologist_4_portrait.png");
			break;
		case h_zaji://LEVEL 2
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/zaji_portrait.png");
			break;
		case h_jido:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/supervisor_portrait.png");
			break;
		case h_josi:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/supervisor_portrait.png");
			break;
		case juji_villager_1:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/archaeologist_1_portrait.png");
			break;
		case juji_villager_2:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/archaeologist_1_portrait.png");
			break;
		case juji_villager_3:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/archaeologist_1_portrait.png");
			break;
		case juji_villager_4:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/archaeologist_1_portrait.png");
			break;
		case h_zurshi://LEVEL 3...
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/giovanni_portrait.png");
			break;
		case zakra_spearman:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/giovanni_portrait.png");
			break;
		default:
			temp.texture = asset_controller::load_texture("assets/graphics/characters/portraits/default.png");
			break;
		}

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
