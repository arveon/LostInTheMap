#include "xml_system.h"

const std::string xml_system::core_config_filepath = "config/core_config.xml";

xml_system::WindowConfig xml_system::load_config_file()
{
	WindowConfig cfg;
	cfg.fullscreen = false;

	std::vector<char> filecontents = xml_system::get_file_content_string(core_config_filepath);

	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

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

	std::vector<char> filecontents = xml_system::get_file_content_string("config/menus.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

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

//TODO: add layer system to load methods switch(cur_tile->name()) case "terrain" - load tiles, etc 


int ** xml_system::load_map_tiles(levels level, int* width, int* height, int* tilewidth, int* tileheight, bool is_combat)
{
	int** tilemap = nullptr;

	std::string path = "map.xml";
	if (is_combat)
		path = "map_combat.xml";
	std::string temp = NameToTypeConversion::get_level_path_prefix(level);
	path = temp + path;

	std::vector<char> filecontents = xml_system::get_file_content_string(path);
	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

	*width = std::stoi(doc.first_node("tilemap")->first_attribute("tileswide")->value());
	*height = std::stoi(doc.first_node("tilemap")->first_attribute("tileshigh")->value());
	*tilewidth = std::stoi(doc.first_node("tilemap")->first_attribute("tilewidth")->value());
	*tileheight = std::stoi(doc.first_node("tilemap")->first_attribute("tileheight")->value());


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

int** xml_system::load_map_collisions(levels level, int width, int height, bool is_combat)
{
	int** tilemap = nullptr;
	std::string path;
	if(!is_combat)
		 path = "map.xml";
	else
	{
		path = "map_combat.xml";
	}
	std::string temp = NameToTypeConversion::get_level_path_prefix(level);
	path = temp + path;

	std::vector<char> filecontents = xml_system::get_file_content_string(path);
	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

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
		if (type.compare("0") == 0)
		{
			int x = std::stoi(cur_node->first_attribute("x")->value());
			int y = std::stoi(cur_node->first_attribute("y")->value());
			tilemap[y][x] = 1;
		}
		cur_node = cur_node->next_sibling();
	}

	return tilemap;
}

Actor** xml_system::load_characters_and_objects(levels level, int width, int height)
{
	Actor** result;
	//initialise the tilemap to -1s as it will represent a tile with nothing
	result = new Actor*[height];
	for (int i = 0; i < height; i++)
	{
		result[i] = new Actor[width];
		for (int j = 0; j < width; j++)
		{
			Actor ch;
			result[i][j] = ch;
		}
	}

	std::string path = "characters.xml";
	std::string temp = NameToTypeConversion::get_level_path_prefix(level);
	path = temp + path;

	std::vector<char> filecontents = xml_system::get_file_content_string(path);
	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

	rapidxml::xml_node<>* cur_node = doc.first_node("tilemap")->first_node("layer")->first_node("tile");
	while (cur_node)
	{
		int id = std::stoi(cur_node->first_attribute("tile")->value());

		int x = std::stoi(cur_node->first_attribute("x")->value());
		int y = std::stoi(cur_node->first_attribute("y")->value());
		result[y][x].value = id;
		rapidxml::xml_attribute<char>* type_attr = cur_node->first_attribute("type");
		if (type_attr != nullptr)
		{
			result[y][x].type = type_attr->value();
			if (result[y][x].type.compare("trigger") == 0)
				result[y][x].script = cur_node->first_attribute("script")->value();
			else if (result[y][x].type.compare("i_object") == 0)
			{
				result[y][x].type = cur_node->first_attribute("name")->value();
				result[y][x].script = cur_node->first_attribute("script")->value();
			}
		}
		else
			result[y][x].type = "";

		rapidxml::xml_attribute<char>* army_attr = cur_node->first_attribute("army");
		if (army_attr)
			result[y][x].army = army_attr->value();
		cur_node = cur_node->next_sibling("tile");
	}


	return result;
}

std::vector<xml_system::LoadingState> xml_system::get_loading_states()
{
	std::vector<xml_system::LoadingState> states;

	std::vector<char> filecontents = xml_system::get_file_content_string("config/loading_states.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

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

xml_system::DialogueFrame xml_system::load_dialogue_frame()
{
	DialogueFrame result;
	std::vector<char> filecontents = xml_system::get_file_content_string("config/dialogue_layout.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

	rapidxml::xml_node<>* bg = doc.first_node("dialog_window")->first_node("bg");
	result.bg_path = bg->first_attribute("image")->value();
	result.bg_frame.x = std::stoi(bg->first_attribute("x")->value());
	result.bg_frame.y = std::stoi(bg->first_attribute("y")->value());
	result.bg_frame.w = std::stoi(bg->first_attribute("w")->value());
	result.bg_frame.h = std::stoi(bg->first_attribute("h")->value());

	bg = bg->next_sibling("portrait");
	result.portrait_frame.x = std::stoi(bg->first_attribute("x")->value());
	result.portrait_frame.y = std::stoi(bg->first_attribute("y")->value());
	result.portrait_frame.w = std::stoi(bg->first_attribute("w")->value());
	result.portrait_frame.h = std::stoi(bg->first_attribute("h")->value());

	bg = bg->next_sibling("text");
	result.text_frame.x = std::stoi(bg->first_attribute("x")->value());
	result.text_frame.y = std::stoi(bg->first_attribute("y")->value());
	result.text_frame.w = std::stoi(bg->first_attribute("w")->value());
	result.text_frame.h = std::stoi(bg->first_attribute("h")->value());

	return result;
}

Script xml_system::load_script(std::string name, levels level)
{
	Script result;

	std::string temp = NameToTypeConversion::get_level_path_prefix(level);
	name = "scripts/" + name;
	name = temp + name;

	try
	{
		std::vector<char> filecontents = xml_system::get_file_content_string(name);
		rapidxml::xml_document<> doc;
		doc.parse<0>(&filecontents[0]);

		rapidxml::xml_node<>* cur_node = doc.first_node("script");

		std::string block = cur_node->first_attribute("block_player")->value();
		if (block.compare("true") == 0)
			result.blocks_player = true;
		else
			result.blocks_player = false;

		rapidxml::xml_attribute<>* o = cur_node->first_attribute("once");
		if (o)
		{
			std::string once = o->value();
			if (once.compare("true") == 0)
				result.happens_once = true;
			else
				result.happens_once = false;
		}
		else
			result.happens_once = true;

		cur_node = cur_node->first_node("action");
		while (cur_node)
		{
			Action temp;
			std::string type = cur_node->first_attribute("type")->value();
			if (type.compare("move") == 0)
			{
				temp.type = action_type::movement;
				std::string character = cur_node->first_attribute("character")->value();
				temp.target_type = NameToTypeConversion::get_character_type_by_name(character);
				int x = std::stoi(cur_node->first_attribute("dest_x")->value());
				int y = std::stoi(cur_node->first_attribute("dest_y")->value());
				temp.movement_dest = { x, y };
				temp.dialogue_path = "";
			}
			else if (type.compare("dialogue") == 0)
			{
				temp.type = action_type::dialogue;
				temp.dialogue_path = cur_node->first_attribute("dialogue")->value();
				temp.movement_dest = { -1,-1 };
				temp.target_type = character_type::none;
			}
			else if (type.compare("wait") == 0)
			{
				temp.type = action_type::wait;
				temp.time = std::stoi(cur_node->first_attribute("time")->value());
				rapidxml::xml_attribute<>* ch = cur_node->first_attribute("character");
				if (ch)
				{
					std::string character = ch->value();
					temp.target_type = NameToTypeConversion::get_character_type_by_name(character);
				}
				else
					temp.target_type = character_type::none;
			}
			else if (type.compare("camera_target") == 0)
			{
				temp.type = action_type::change_camera_target;
				std::string character = cur_node->first_attribute("character")->value();
				temp.target_type = NameToTypeConversion::get_character_type_by_name(character);
			}
			else if (type.compare("move_camera") == 0)
			{
				temp.type = action_type::move_camera_to_tile;
				int x = std::stoi(cur_node->first_attribute("dest_x")->value());
				int y = std::stoi(cur_node->first_attribute("dest_y")->value());
				temp.movement_dest = { x, y };
			}
			else if (type.compare("start_combat") == 0)
			{
				temp.type = action_type::start_combat;
			}
			else if (type.compare("set_story_state") == 0)
			{
				temp.type = action_type::set_story_state;
				temp.utility = cur_node->first_attribute("new_state")->value();
			}
			else if (type.compare("object_disappear") == 0)
			{
				temp.type = action_type::object_disappear;
				temp.utility = cur_node->first_attribute("object_type")->value();
			}
			else if (type.compare("character_disappear") == 0)
			{
				temp.type = action_type::character_disappear;
				temp.target_type = NameToTypeConversion::get_character_type_by_name(cur_node->first_attribute("character")->value());
			}
			else if (type.compare("change_level") == 0)
			{
				temp.type = action_type::level_switch;
				temp.utility = cur_node->first_attribute("level")->value();
			}
			else if (type.compare("join_army") == 0)
			{
				temp.type = action_type::join_army;
				temp.target_type = NameToTypeConversion::get_character_type_by_name(cur_node->first_attribute("character")->value());
				temp.num_utility = std::stoi(cur_node->first_attribute("quantity")->value());
			}
			else if (type.compare("fadein") == 0)
			{
				temp.type = action_type::fade_in;
				temp.time = std::stoi(cur_node->first_attribute("time")->value());

				rapidxml::xml_attribute<>* wait = cur_node->first_attribute("wait_time");
				if(wait)
					temp.num_utility = std::stoi(wait->value());
			}
			else if (type.compare("fadeout") == 0)
			{
				temp.type = action_type::fade_out;
				temp.time = std::stoi(cur_node->first_attribute("time")->value());
			}
			else if (type.compare("animate_object") == 0)
			{
				temp.type = action_type::animate_object;
				temp.utility = cur_node->first_attribute("object")->value();
			}
			else
				temp.type = action_type::not_set;

			result.actions.push_back(temp);
			cur_node = cur_node->next_sibling("action");
		}
		result.initialised = true;
	}
	catch (std::exception e)
	{
		std::cout << "script not found at " << name << std::endl;
		result.initialised = false;
		return result;
	}

	
	return result;
}

xml_system::Dialogue xml_system::load_dialogue(levels level, std::string path)
{
	Dialogue result;

	std::string final_path = NameToTypeConversion::get_level_path_prefix(level);
	final_path = final_path + "dialogs/" + path;

	std::vector<char> filecontents = xml_system::get_file_content_string(final_path);
	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

	rapidxml::xml_node<>* character = doc.first_node("dialogue");

	//load character types participating in the dialogue
	rapidxml::xml_attribute<>* atr = character->first_attribute("character");
	while (atr)
	{
		std::string name = atr->value();
		result.characters.push_back(NameToTypeConversion::get_character_type_by_name(name));


		atr = atr->next_attribute("character");
	}

	//load all required replicas
	rapidxml::xml_node<>* replica = character->first_node("rep");
	while (replica)
	{
		DialogueLine line;
		line.text = replica->first_attribute("text")->value();
		int id = std::stoi(replica->first_attribute("character")->value());
		line.character = result.characters.at(id);
		//std::cout << line.character << ": " << line.text << std::endl;
		result.lines.push_back(line);
		replica = replica->next_sibling("rep");
	}
	result.initialised = true;
	return result;
}

std::vector<army_unit*> xml_system::load_army(std::string army_path, levels level, bool is_enemy)
{
	std::vector<army_unit*> result;

	std::string path = NameToTypeConversion::get_level_path_prefix(level) + "armies/" + army_path.c_str();
	std::vector<char> filecontents = xml_system::get_file_content_string(path);
	rapidxml::xml_document<> doc;
	doc.parse<0>(&filecontents[0]);

	rapidxml::xml_node<>* unit = doc.first_node("army")->first_node("unit");
	//load unit types and quantities
	while (unit)
	{
		army_unit* u = new army_unit();
		std::string name = unit->first_attribute("type")->value();
		u->type = NameToTypeConversion::get_character_type_by_name(name);
		u->quantity = std::stoi(unit->first_attribute("quantity")->value());
		result.push_back(u);
		unit = unit->next_sibling("unit");
	}

	//construct the vector of units that need to be loaded
	std::vector<character_type> units_to_be_loaded;
	for (army_unit* u : result)
	{
		bool already_exists = false;
		for (character_type existing_type : units_to_be_loaded)
		{
			if (u->type == existing_type)
			{
				already_exists = true;
				continue;
			}
		}
		if (!already_exists)
			units_to_be_loaded.push_back(u->type);
	}

	//load unit stats
	if (units_to_be_loaded.size() > 0)
	{
		for (character_type unit : units_to_be_loaded)
		{
			army_unit* un = xml_system::load_army_unit(unit);

			//apply units stats to all units of that type in the army
			for (army_unit* u : result)
			{
				if (u->type == unit)
				{
					u->health_of_first = un->health_of_first;
					u->max_health = un->max_health;

					u->min_damage_close = un->min_damage_close;
					u->max_damage_close = un->max_damage_close;

					u->min_damage_ranged = un->min_damage_ranged;
					u->max_damage_ranged = un->max_damage_ranged;
					u->ranged_allowed = un->ranged_allowed;

					u->speed = un->speed;
					u->is_enemy = is_enemy;
				}
			}
			delete un;
		}
	}

	return result;
}

army_unit* xml_system::load_army_unit(character_type ch)
{
	//preload units file
	std::vector<char> filecontents = xml_system::get_file_content_string("config/units.xml");
	rapidxml::xml_document<> units_doc;
	units_doc.parse<0>(&filecontents[0]);

	army_unit* u = new army_unit();
	std::string name = NameToTypeConversion::get_character_name_by_type(ch);
	rapidxml::xml_node<>* unit_data = units_doc.first_node(name.c_str())->first_node();

	//read in units stats of the given unit
	while (unit_data)
	{
		std::string data_name = unit_data->name();
		if (data_name.compare("health") == 0)
		{
			u->health_of_first = std::stoi(unit_data->first_attribute("value")->value());
			u->max_health = std::stoi(unit_data->first_attribute("value")->value());
		}
		else if (data_name.compare("close_attack") == 0)
		{
			u->min_damage_close = std::stoi(unit_data->first_attribute("min_value")->value());
			u->max_damage_close= std::stoi(unit_data->first_attribute("max_value")->value());
		}
		else if (data_name.compare("ranged_attack") == 0)
		{
			std::string val = unit_data->first_attribute("allowed")->value();
			if (val.compare("true") == 0)
			{
				u->ranged_allowed = true;
				u->min_damage_ranged= std::stoi(unit_data->first_attribute("min_value")->value());
				u->max_damage_ranged= std::stoi(unit_data->first_attribute("max_value")->value());
			}
		}
		else if (data_name.compare("speed") == 0)
		{
			u->speed= std::stoi(unit_data->first_attribute("value")->value());
		}
		unit_data = unit_data->next_sibling();
	}

	return u;
}

std::vector<char> xml_system::get_file_content_string(std::string path)
{
	//load file into a string
	std::string result_str;
	std::ifstream is(path);
	result_str.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

	//result_str = Encoder::decode_base64(result_str);

	//copy string into a vector of characters and add a null terminator at the end
	std::vector<char> result(result_str.begin(), result_str.end());
	result.push_back('\0');

	return result;
}

xml_system::xml_system()
{
}


xml_system::~xml_system()
{
}
