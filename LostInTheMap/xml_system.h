#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <rapidxml\rapidxml.hpp>
#include <rapidxml\rapidxml_utils.hpp>

#include "map_system.h"

#include "Components.h"
#include "MenuLayout.h"

class xml_system
{
private:
	const std::string core_config_filepath = "config/core_config";
public:
	typedef struct
	{
		std::string title;
		int width;
		int height;
		bool fullscreen;
	} WindowConfig;

	typedef struct
	{
		std::string name;
		int value;
	} LoadingState;

	typedef struct
	{
		std::string text;
		character_type character;
	}DialogueLine;

	typedef struct
	{
		bool initialised = false;
		std::vector<DialogueLine> lines;
	} Dialogue;

	static WindowConfig load_config_file();
	static MenuLayout load_interface_layout(std::string name);
	static int** load_map_tiles(levels level, int* width, int* height, int* tilewidth);
	static int** load_map_collisions(levels level, int width, int height);
	static Character** load_characters(levels level, int width, int height);
	static std::vector<LoadingState> get_loading_states();

	static Dialogue load_dialogue(std::string path);

	xml_system();
	~xml_system();

	
};

