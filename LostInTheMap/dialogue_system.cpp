#include "dialogue_system.h"

xml_system::Dialogue dialogue_system::cur_dialogue;
int dialogue_system::counter;
int dialogue_system::line_counter;
int dialogue_system::elapsed_char_time;
int dialogue_system::required_char_time = 10;

SDL_Texture* dialogue_system::cur_texture;

void dialogue_system::inc_sprite()
{
	if (cur_texture != nullptr && counter != cur_dialogue.lines.at(line_counter).text.length())
		asset_controller::destroy_texture(cur_texture);
	std::string text = cur_dialogue.lines.at(line_counter).text.substr(0, counter);
	//std::cout << text << std::endl;
	cur_texture = asset_controller::get_texture_from_text(text.c_str(), UI_text_type::game_dialog);
}

void dialogue_system::start_dialogue(xml_system::Dialogue dial)
{
	elapsed_char_time = 0;
	line_counter = 0;
	cur_dialogue = dial;
	counter = 1;
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

dialogue_system::dialogue_system()
{
}


dialogue_system::~dialogue_system()
{
}
