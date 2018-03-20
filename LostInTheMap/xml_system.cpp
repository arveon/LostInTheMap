#include "xml_system.h"

xml_system::WindowConfig xml_system::load_config_file()
{
	WindowConfig cfg;
	cfg.fullscreen = false;

	rapidxml::file<> file("config/core_config.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

	rapidxml::xml_node<>* cur_node = doc.first_node("window_config")->first_node();

	while (cur_node != NULL)
	{
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
	doc.clear();
	return cfg;
}

MenuLayout xml_system::load_interface_layout(std::string name)
{
	MenuLayout layout;

	rapidxml::file<> file("config/menus.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

	rapidxml::xml_node<>* cur_node = doc.first_node(name.c_str());
	layout.background_path = cur_node->first_attribute("background")->value();
	cur_node = cur_node->first_node();
	//cur_node = cur_node->next_sibling();
	while (cur_node != NULL)
	{
		ui_element_config elem;
		std::string name = cur_node->name();
		//std::strcmp(name, "slider")
		
		if (name.compare("slider") == 0)
		{
			elem = Slider(std::stoi(cur_node->first_attribute("value")->value()));
		}
		else if (name.compare("button") == 0)
		{
			elem = Button();
		}
		else if (name.compare("text") == 0)
		{
			elem = Text();
		}
		else if (name.compare("bar") == 0)
		{
			elem = Bar();
		}

		elem.name = cur_node->first_attribute("text")->value();
		elem.position = { std::stoi(cur_node->first_attribute("x")->value()), std::stoi(cur_node->first_attribute("y")->value()) };

		layout.buttons.push_back(elem);

		cur_node = cur_node->next_sibling();
	}

	doc.clear();
	return layout;
}

int ** xml_system::load_map_tiles(levels level, int* width, int* height, int* tilewidth)
{
	int** tilemap = nullptr;

	std::string path = "map.xml";
	switch (level)
	{
	case levels::test:
		path = "Levels/tests/" + path;
		break;
	case levels::pyramid:
		path = "Levels/pyramid/" + path;
		break;

	}

	rapidxml::file<> file(path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

	*width = std::stoi(doc.first_node("tilemap")->first_attribute("tileswide")->value());
	*height = std::stoi(doc.first_node("tilemap")->first_attribute("tileshigh")->value());
	*tilewidth = std::stoi(doc.first_node("tilemap")->first_attribute("tilewidth")->value());


	//initialise the tilemap to -1s as it will represent a tile with nothing
	tilemap = new int*[*height];
	for (int i = 0; i < *height; i++)
	{
		tilemap[i] = new int[*width];
		for (int j = 0; j < *width; j++)
		{
			tilemap[i][j] = -1;
		}
	}
	
	//loop through all nodes and get the tile values from them
	rapidxml::xml_node<>* cur_node = doc.first_node("tilemap")->first_node("layer")->first_node("tile");
	while (cur_node)
	{
		int x = std::stoi(cur_node->first_attribute("x")->value());
		int y = std::stoi(cur_node->first_attribute("y")->value());

		tilemap[y][x] = std::stoi(cur_node->first_attribute("tile")->value());
		cur_node = cur_node->next_sibling();
	}

	return tilemap;
}

int** xml_system::load_map_collisions(levels level, int width, int height)
{
	int** tilemap = nullptr;
	std::string path = "map.xml";
	switch (level)
	{
	case levels::test:
		path = "Levels/tests/"+path;
		break;
	case levels::pyramid:
		path = "Levels/pyramid/" + path;
		break;

	}

	rapidxml::file<> file(path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

	//initialise the tilemap to -1s as it will represent a tile with nothing
	tilemap = new int*[height];
	for (int i = 0; i < height; i++)
	{
		tilemap[i] = new int[width];
		for (int j = 0; j < width; j++)
			tilemap[i][j] = 0;
	}

	rapidxml::xml_node<>* cur_node = doc.first_node("tilemap")->first_node("layer")->next_sibling()->first_node("tile");
	while (cur_node)
	{
		std::string type = cur_node->first_attribute("tile")->value();
		if (type.compare("1"))
		{
			int x = std::stoi(cur_node->first_attribute("x")->value());
			int y = std::stoi(cur_node->first_attribute("y")->value());
			tilemap[y][x] = 1;
		}
		cur_node = cur_node->next_sibling();
	}

	return tilemap;
}

std::vector<xml_system::LoadingState> xml_system::get_loading_states()
{
	std::vector<xml_system::LoadingState> states;
	
	rapidxml::file<> file("config/loading_states.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());
	
	rapidxml::xml_node<>* cur_node = doc.first_node("state");
	while (cur_node)
	{
		std::string name = cur_node->name();
		if (name.compare("state") == 0)
			states.push_back({ cur_node->first_attribute("text")->value(), std::stoi(cur_node->first_attribute("value")->value()) });
		cur_node = cur_node->next_sibling();
	}

	return states;
}

xml_system::xml_system()
{
}


xml_system::~xml_system()
{
}
