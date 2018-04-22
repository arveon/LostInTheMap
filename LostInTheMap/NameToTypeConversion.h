#pragma once
#include "CharacterTypes.h"
#include "Levels.h"
#include "ObjectTypes.h"
#include <string>
class NameToTypeConversion
{
public:
	NameToTypeConversion();
	~NameToTypeConversion();

	static character_type get_character_type_by_name(std::string);
	static std::string get_character_name_by_type(character_type type);
	static std::string get_level_path_prefix(levels level);
	static std::string get_level_name_str(levels level);

	static levels get_level_type_from_name(std::string name);

	static object_types get_object_type_by_name(std::string name);

	static std::string get_object_name_by_type(object_types type);
};

