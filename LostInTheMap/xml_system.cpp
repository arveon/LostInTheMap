#include "xml_system.h"



xml_system::WindowConfig xml_system::load_config_file()
{
	WindowConfig cfg;
	cfg.fullscreen = false;

	rapidxml::file<> file("config/core_config.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

	rapidxml::xml_node<>* cur_node = doc.first_node("window_config")->first_node();
	//cfg.title = cur_node->first_attribute("value")->value();

	/*cur_node = cur_node->next_sibling("resolution");
	cfg.width = std::stoi(cur_node->first_attribute("width")->value());
	cfg.height = std::stoi(cur_node->first_attribute("height")->value());

	cur_node = cur_node->first_node("fullscreen");
	cfg.fullscreen = cur_node->first_attribute("value")->value();*/


	while (cur_node != NULL)
	{
		std::cout << cur_node->name() << std::endl;
		if (std::string(cur_node->name()) == "title")
		{
			cfg.title = cur_node->first_attribute("value")->value();
		}
		else if (std::string(cur_node->name()) == "resolution")
		{
			cfg.width = std::stoi(cur_node->first_attribute("width")->value());
			cfg.height = std::stoi(cur_node->first_attribute("height")->value());
		}
		else if (std::string(cur_node->name()) == "fullscreen")
		{
			if (std::string(cur_node->first_attribute("value")->value()) == "true")
				cfg.fullscreen = true;
			else
				cfg.fullscreen = false;
		}
		

		cur_node = cur_node->next_sibling();
	}

	return cfg;
}

xml_system::xml_system()
{
}


xml_system::~xml_system()
{
}
