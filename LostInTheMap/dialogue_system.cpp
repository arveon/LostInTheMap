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
Entity* dialogue_system::cur_dialogue_target;

void dialogue_system::inc_sprite()
{
	//don't destroy texture for the last version of text
	if (cur_texture != nullptr && counter < (int)cur_dialogue.lines.at(line_counter).text.length())
		asset_controller::destroy_texture(cur_texture);
	std::string text = cur_dialogue.lines.at(line_counter).text.substr(0, counter);
	cur_texture = asset_controller::get_texture_from_text(text.c_str(), UI_text_type::game_dialog, max_text_width);
}

void dialogue_system::start_dialogue(xml_system::Dialogue dial, std::vector<asset_controller::CharacterPortrait> portraits, Entity* target)
{
	elapsed_char_time = 0;
	line_counter = 0;
	cur_dialogue = dial;
	counter = 1;

	cur_dialogue_target = target;

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

void dialogue_system::next_line()
{
	counter = 1;
	if (line_counter >= (int)cur_dialogue.lines.size() - 1)
	{//finished
		asset_controller::destroy_texture(cur_texture);
		cur_dialogue.initialised = false;
		if(cur_dialogue_target)
			character_system::allow_character_movement(cur_dialogue_target);
		if (cur_dialogue.requires_callback)
			cur_dialogue.callback(nullptr);
	}
	else
	{
		line_counter++;
		dialogue_system::inc_sprite();
		dialogue_system::update_portrait();
	}
}

void dialogue_system::update(int dt)
{
	elapsed_char_time += dt;
	if (elapsed_char_time >= required_char_time)
	{
		elapsed_char_time = 0;
		if (counter <= (int)cur_dialogue.lines.at(line_counter).text.length())
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
