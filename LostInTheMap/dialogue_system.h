#pragma once
#include <vector>
#include "Entity.h"

#include "xml_system.h"

class dialogue_system
{
private:
	static xml_system::Dialogue cur_dialogue;
	static int counter;
	static int line_counter;

	static int elapsed_char_time;
	static int required_char_time;
public:
	static void start_dialogue(xml_system::Dialogue);
	static std::string get_cur_line() { return cur_dialogue.lines.at(line_counter).text.substr(0, counter); }
	static bool is_line_done() { return (counter > cur_dialogue.lines.at(line_counter).text.length()); }

	static void next_line() 
	{ 
		counter = 0; 
		if (line_counter >= cur_dialogue.lines.size()-1) 
			cur_dialogue.initialised = false; 
		else 
			line_counter++;
	}

	static bool dialogue_pending() { return cur_dialogue.initialised; }

	static void update(int dt);

	dialogue_system();
	~dialogue_system();
};

