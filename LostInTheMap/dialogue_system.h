#pragma once
#include <vector>
#include "Entity.h"

#include "xml_system.h"
#include "CharacterTypes.h"

class dialogue_system
{
private:
	static xml_system::Dialogue cur_dialogue;
	static int counter;
	static int line_counter;
	static SDL_Texture* cur_texture;
	static SDL_Texture* cur_portrait;
	static std::vector<asset_controller::CharacterPortrait> portraits;

	static int elapsed_char_time;
	static int required_char_time;
	static void inc_sprite();
public:
	static int max_text_width;
	static void start_dialogue(xml_system::Dialogue, std::vector<asset_controller::CharacterPortrait>);
	static std::string get_cur_line() { return cur_dialogue.lines.at(line_counter).text.substr(0, counter); }
	static bool is_line_done() { return (counter > cur_dialogue.lines.at(line_counter).text.length()); }
	static void finish_line() { counter = cur_dialogue.lines.at(line_counter).text.size(); inc_sprite(); }

	static SDL_Texture* get_cur_line_sprite() {	return cur_texture;	}
	static SDL_Texture* get_cur_portrait() { return cur_portrait; }


	static void next_line() 
	{ 
		counter = 1; 
		if (line_counter >= cur_dialogue.lines.size() - 1)
		{
			asset_controller::destroy_texture(cur_texture);
			cur_dialogue.initialised = false;
		}
		else
		{
			line_counter++;
			dialogue_system::inc_sprite();
			dialogue_system::update_portrait();
		}
		
	}
	static bool dialogue_pending() { return cur_dialogue.initialised; }

	static void update(int dt);
	static void update_portrait();

	dialogue_system();
	~dialogue_system();
};

