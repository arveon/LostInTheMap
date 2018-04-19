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

//TODO: add layer system to load methods switch(cur_tile->name()) case "terrain" - load tiles, etc 


int ** xml_system::load_map_tiles(levels level, int* width, int* height, int* tilewidth, bool is_combat)
{
	int** tilemap = nullptr;

	std::string path = "map.xml";
	std::string temp = xml_system::get_level_path_prefix(level);
	path = temp + path;

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
	std::string temp = xml_system::get_level_path_prefix(level);
	path = temp + path;

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
	std::string temp = xml_system::get_level_path_prefix(level);
	path = temp + path;

	rapidxml::file<> file(path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

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

xml_system::DialogueFrame xml_system::load_dialogue_frame()
{
	DialogueFrame result;
	rapidxml::file<> file("config/dialogue_layout.xml");
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

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

	std::string temp = xml_system::get_level_path_prefix(level);
	name = "scripts/" + name;
	name = temp + name;

	rapidxml::file<> file(name.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

	rapidxml::xml_node<>* cur_node = doc.first_node("script");

	std::string block = cur_node->first_attribute("block_player")->value();
	if (block.compare("true") == 0)
		result.blocks_player = true;
	else
		result.blocks_player = false;

	cur_node = cur_node->first_node("action");
	while (cur_node)
	{
		Action temp;
		std::string type = cur_node->first_attribute("type")->value();
		if (type.compare("move") == 0)
		{
			temp.type = action_type::movement;
			std::string character = cur_node->first_attribute("character")->value();
			temp.target_type = get_character_type_by_name(character);
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
				temp.target_type = get_character_type_by_name(character);
			}
			else
				temp.target_type = character_type::none;
		}
		else if (type.compare("camera_target") == 0)
		{
			temp.type = action_type::change_camera_target;
			std::string character = cur_node->first_attribute("character")->value();
			temp.target_type = get_character_type_by_name(character);
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
		else
			temp.type = action_type::not_set;

		result.actions.push_back(temp);
		cur_node = cur_node->next_sibling("action");
	}


	return result;
}

xml_system::Dialogue xml_system::load_dialogue(std::string path)
{
	Dialogue result;

	rapidxml::file<> file(path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

	rapidxml::xml_node<>* character = doc.first_node("dialogue");

	//load character types participating in the dialogue
	rapidxml::xml_attribute<>* atr = character->first_attribute("character");
	while (atr)
	{
		std::string name = atr->value();
		result.characters.push_back(xml_system::get_character_type_by_name(name));


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

std::vector<army_unit> xml_system::load_army(std::string army_path, levels level)
{
	std::vector<army_unit> result;

	std::string path = xml_system::get_level_path_prefix(level) + "armies/" + army_path.c_str();
	rapidxml::file<> file(path.c_str());
	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());

	rapidxml::xml_node<>* unit = doc.first_node("army")->first_node("unit");
	while (unit)
	{
		army_unit u;
		u.type = xml_system::get_character_type_by_name(unit->first_attribute("type")->value());
		u.quantity = std::stoi(unit->first_attribute("quantity")->value());
		result.push_back(u);
		unit = unit->next_sibling("unit");
	}

	return result;
}

character_type xml_system::get_character_type_by_name(std::string name)
{
	character_type result;
	if (name.compare("giovanni") == 0 || name.compare("player") == 0 || name.compare("") == 0)
		result = character_type::h_giovanni;
	else if (name.compare("arch_supervisor") == 0 || name.compare("npc_arch_supervisor") == 0)//LEVEL 1
		result = character_type::npc_arch_supervisor;
	else if (name.compare("archaeologist_1") == 0 || name.compare("archaeologist") == 0)
		result = character_type::npc_archaeologist_1;
	else if (name.compare("archaeologist_2") == 0)
		result = character_type::npc_archaeologist_2;
	else if (name.compare("archaeologist_3") == 0)
		result = character_type::npc_archaeologist_3;
	else if (name.compare("archaeologist_4") == 0)
		result = character_type::npc_archaeologist_4;
	else if (name.compare("rat") == 0)
		result = character_type::rat;
	else if (name.compare("zaji") == 0)
		result = character_type::h_zaji;//LEVEL 2
	else if (name.compare("jido") == 0)
		result = character_type::h_jido;
	else if (name.compare("josi") == 0)
		result = character_type::h_josi;
	else if (name.compare("juji_villager_1") == 0 || name.compare("juji_villager") == 0)
		result = character_type::juji_villager_1;
	else if (name.compare("juji_villager_2") == 0)
		result = character_type::juji_villager_2;
	else if (name.compare("juji_villager_3") == 0)
		result = character_type::juji_villager_3;
	else if (name.compare("juji_villager_4") == 0)
		result = character_type::juji_villager_4;
	else if (name.compare("snake") == 0)
		result = character_type::snake;
	else if (name.compare("zakra_spearman") == 0)//LEVEL 3
		result = character_type::zakra_spearman;
	else if (name.compare("zurshi") == 0)
		result = character_type::h_zurshi;
	else
		result = character_type::none;

	return result;
}

std::string xml_system::get_level_path_prefix(levels level)
{
	std::string path = "";
	switch (level)
	{
	case levels::test:
		path = "Levels/tests/";
		break;
	case levels::pyramid:
		path = "Levels/pyramid/";
		break;
	case levels::juji_village:
		path = "Levels/juji_village/";
		break;
	case levels::caves:
		path = "Levels/bodah_caves/";
		break;
	case levels::zakra_village:
		path = "Levels/zakra_village/";
		break;
	case levels::desert:
		path = "Levels/bone_fields/";
		break;
	default:
		path = "Levels/pyramid/";
	}
	return path;
}

std::string xml_system::get_level_name_str(levels level)
{
	std::string path = "";
	switch (level)
	{
	case levels::test:
		path = "test";
		break;
	case levels::pyramid:
		path = "pyramid";
		break;
	case levels::juji_village:
		path = "juji_village";
		break;
	case levels::caves:
		path = "bodah_caves";
		break;
	case levels::zakra_village:
		path = "juji_village";
		break;
	case levels::desert:
		path = "bone_fields";
		break;
	default:
		path = "pyramid";
	}
	return path;
}

object_types xml_system::get_object_type_by_name(std::string name)
{
	object_types result;
	if (name.compare("lever") == 0)
		result = object_types::lever;
	else if (name.compare("juji_house_1") == 0 || name.compare("juji_house") == 0)
		result = object_types::juji_house_1;
	else if (name.compare("juji_house_2") == 0)
		result = object_types::juji_house_2;
	else if (name.compare("blowgun") == 0)
		result = object_types::pickup_blowgun;
	else if (name.compare("sling") == 0)
		result = object_types::pickup_sling;
	else if (name.compare("gold_pile") == 0)
		result = object_types::pickup_gold_pile;
	else if (name.compare("spear") == 0)
		result = object_types::pickup_spear;
	else if (name.compare("sword") == 0)
		result = object_types::pickup_sword;

	return result;
}

std::string xml_system::get_object_name_by_type(object_types type)
{
	std::string result = "";
	switch (type)
	{
	case object_types::juji_house_1:
		result = "juji_house_1";
		break;
	case object_types::juji_house_2:
		result = "juji_house_2";
		break;
	case object_types::lever:
		result = "lever";
		break;
	case object_types::pickup_blowgun:
		result = "blowgun";
		break;
	case object_types::pickup_gold_pile:
		result = "gold_pile";
		break;
	case object_types::pickup_sling:
		result = "sling";
		break;
	case object_types::pickup_spear:
		result = "spear";
		break;
	case object_types::pickup_sword:
		result = "sword";
		break;
	}

	return result;
}



xml_system::xml_system()
{
}


xml_system::~xml_system()
{
}
