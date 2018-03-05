#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <rapidxml\rapidxml.hpp>
#include <rapidxml\rapidxml_utils.hpp>

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

	static WindowConfig load_config_file();
	static MenuLayout load_menu_layout();
	static MenuLayout load_settings_layout();
	xml_system();
	~xml_system();

	
};

