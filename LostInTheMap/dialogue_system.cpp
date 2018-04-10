#include "dialogue_system.h"

xml_system::Dialogue dialogue_system::cur_dialogue;
int dialogue_system::counter;
int dialogue_system::line_counter;
int dialogue_system::elapsed_char_time;
int dialogue_system::required_char_time = 20;
int dialogue_system::max_text_width;
SDL_Texture* dialogue_system::cur_texture;
SDL_Texture* dialogue_system::cur_portrait;
std::vector<asset_controller::CharacterPortrait> dialogue_system::portraits;

void dialogue_system::inc_sprite()
{
	//don't destroy texture for the last version of text
	if (cur_texture != nullptr && counter < cur_dialogue.lines.at(line_counter).text.length())
		asset_controller::destroy_texture(cur_texture);
	std::string text = cur_dialogue.lines.at(line_counter).text.substr(0, counter);
	cur_texture = asset_controller::get_texture_from_text(text.c_str(), UI_text_type::game_dialog, max_text_width);
}

void dialogue_system::start_dialogue(xml_system::Dialogue dial, std::vector<asset_controller::CharacterPortrait> portraits)
{
	elapsed_char_time = 0;
	line_counter = 0;
	cur_dialogue = dial;
	counter = 1;

	while(dialogue_system::portraits.size() > 0)
	{
		SDL_Texture* temp = dialogue_system::portraits.back().texture;
		asset_controller::destroy_texture(temp);
		dialogue_system::portraits.pop_back();
	}

	dialogue_system::portraits.clear();
	dialogue_system::portraits = portraits;
	update_portrait();
}

void dialogue_system::update(int dt)
{
	elapsed_char_time += dt;
	if (elapsed_char_time >= required_char_time)
	{
		elapsed_char_time = 0;
		if (counter <= cur_dialogue.lines.at(line_counter).text.length())
		{
			inc_sprite();
			counter++;
		}
	}

}

void dialogue_system::update_portrait()
{
	character_type cur_char = cur_dialogue.lines.at(line_counter).character;
	for (unsigned int i = 0; i < portraits.size(); i++)
	{
		if (portraits.at(i).ch_type == cur_char)
			cur_portrait = portraits.at(i).texture;
	}

}

dialogue_system::dialogue_system()
{
}


dialogue_system::~dialogue_system()
{
}
