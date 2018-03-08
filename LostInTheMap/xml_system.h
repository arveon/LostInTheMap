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

	static WindowConfig load_config_file();
	static MenuLayout load_interface_layout(std::string name);
	static int** load_map_tiles(levels level, int* width, int* height);
	static std::vector<LoadingState> get_loading_states();
	xml_system();
	~xml_system();

	
};

