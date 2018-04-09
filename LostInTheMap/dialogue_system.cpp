#include "dialogue_system.h"

xml_system::Dialogue dialogue_system::cur_dialogue;
int dialogue_system::counter;
int dialogue_system::line_counter;
int dialogue_system::elapsed_char_time;
int dialogue_system::required_char_time = 10;

void dialogue_system::start_dialogue(xml_system::Dialogue dial)
{
	elapsed_char_time = 0;
	line_counter = 0;
	cur_dialogue = dial;
	counter = 0;
}

void dialogue_system::update(int dt)
{
	elapsed_char_time += dt;
	if (elapsed_char_time >= required_char_time)
	{
		elapsed_char_time = 0;
		if (counter <= cur_dialogue.lines.at(line_counter).text.length())
			counter++;
	}

}

dialogue_system::dialogue_system()
{
}


dialogue_system::~dialogue_system()
{
}
