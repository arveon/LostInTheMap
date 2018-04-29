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

#include "NameToTypeConversion.h"

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
	static int** load_map_tiles(levels level, int* width, int* height, int* tilewidth, int* tileheight, bool is_combat = false);
	static int** load_map_collisions(levels level, int width, int height, bool is_combat = false);
	static Actor** load_characters_and_objects(levels level, int width, int height);
	static std::vector<LoadingState> get_loading_states();
	static DialogueFrame load_dialogue_frame();
	static Script load_script(std::string name, levels level);

	static std::vector<army_unit*> load_army(std::string army_path, levels level, bool is_enemy = true);

	static Dialogue load_dialogue(levels level, std::string path);

	xml_system();
	~xml_system();

	
};

