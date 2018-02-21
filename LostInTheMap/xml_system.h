#pragma once
#include <string>
#include <iostream>

#include <rapidxml\rapidxml.hpp>
#include <rapidxml\rapidxml_utils.hpp>

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
	xml_system();
	~xml_system();

	
};

