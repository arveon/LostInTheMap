#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <rapidxml\rapidxml.hpp>
#include <rapidxml\rapidxml_utils.hpp>

#include "Levels.h"
#include "Script.h"

#include "Components.h"
#include "MenuLayout.h"

#include "ArmyUnit.h"

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
	} DialogueLine;

	typedef struct
	{
		bool initialised = false;
		bool requires_callback = false;
		void(*callback)(Entity* performer);
		std::vector<character_type> characters;
		std::vector<DialogueLine> lines;
	} Dialogue;

	typedef struct
	{
		SDL_Rect bg_frame;
		std::string bg_path;
		SDL_Rect portrait_frame;
		SDL_Rect text_frame;
	} DialogueFrame;

	static WindowConfig load_config_file();
	static MenuLayout load_interface_layout(std::string name);
	static int** load_map_tiles(levels level, int* width, int* height, int* tilewidth);
	static int** load_map_collisions(levels level, int width, int height);
	static Actor** load_characters_and_objects(levels level, int width, int height);
	static std::vector<LoadingState> get_loading_states();
	static DialogueFrame load_dialogue_frame();
	static Script load_script(std::string name, levels level);

	static character_type get_character_type_by_name(std::string);
	static std::string get_level_path_prefix(levels level);
	static std::string get_level_name_str(levels level);

	static object_types get_object_type_by_name(std::string name);

	static std::string get_object_name_by_type(object_types type);

	static std::vector<army_unit> load_army(std::string army_path, levels level);

	static Dialogue load_dialogue(std::string path);

	xml_system();
	~xml_system();

	
};

