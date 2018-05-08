#include "NameToTypeConversion.h"



NameToTypeConversion::NameToTypeConversion()
{
}


NameToTypeConversion::~NameToTypeConversion()
{
}

character_type NameToTypeConversion::get_character_type_by_name(std::string name)
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

std::string NameToTypeConversion::get_character_name_by_type(character_type type)
{
	std::string result;
	switch (type)
	{
	case character_type::h_giovanni:
		result = "giovanni";
		break;
	case character_type::h_jido:
		result = "jido";
		break;
	case character_type::h_josi:
		result = "josi";
		break;
	case character_type::h_zaji:
		result = "zaji";
		break;
	case character_type::h_zurshi:
		result = "zurshi";
		break;
	case character_type::juji_spearman:
		result = "juji_spearman";
		break;
	case character_type::juji_villager_1:
		result = "juji_villager_1";
		break;
	case character_type::juji_villager_2:
		result = "juji_villager_2";
		break;
	case character_type::juji_villager_3:
		result = "juji_villager_3";
		break;
	case character_type::juji_villager_4:
		result = "juji_villager_4";
		break;
	case character_type::npc_archaeologist_1:
		result = "archaeologist_1";
		break;
	case character_type::npc_archaeologist_2:
		result = "archaeologist_2";
		break;
	case character_type::npc_archaeologist_3:
		result = "archaeologist_3";
		break;
	case character_type::npc_archaeologist_4:
		result = "archaeologist_4";
		break;
	case character_type::npc_arch_supervisor:
		result = "arch_supervisor";
		break;
	case character_type::rat:
		result = "rat";
		break;
	case character_type::zakra_spearman:
		result = "zakra_spearman";
		break;
	}
	return result;
}

std::string NameToTypeConversion::get_level_path_prefix(levels level)
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
	case levels::pyramid_room:
		path = "Levels/pyramid_room/";
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

std::string NameToTypeConversion::get_level_name_str(levels level)
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

levels NameToTypeConversion::get_level_type_from_name(std::string name)
{
	levels result;
	if (name.compare("pyramid") == 0)
		result = levels::pyramid;
	else if (name.compare("pyramid_room") == 0)
		result = levels::pyramid_room;
	else if (name.compare("juji_village") == 0)
		result = levels::juji_village;
	else if (name.compare("bodah_caves") == 0)
		result = levels::caves;
	else if (name.compare("zakra_village") == 0)
		result = levels::zakra_village;
	else if (name.compare("desert") == 0)
		result = levels::desert;
	else
		result = levels::pyramid;

	return result;
}

object_types NameToTypeConversion::get_object_type_by_name(std::string name)
{
	object_types result;
	if (name.compare("lever") == 0)
		result = object_types::lever;
	else if (name.compare("hidden_door") == 0)
		result = object_types::hidden_door;
	else if (name.compare("map") == 0)
		result = object_types::map;
	else if (name.compare("juji_house_1") == 0 || name.compare("juji_house") == 0)
		result = object_types::juji_house_1;
	else if (name.compare("juji_house_2") == 0)
		result = object_types::juji_house_2;
	else if (name.compare("chest") == 0)
		result = object_types::chest;
	else if (name.compare("barrel") == 0)
		result = object_types::barrel;
	else if (name.compare("boards_vertical") == 0)
		result = object_types::boards_vertical;
	else if (name.compare("weapon_rack") == 0)
		result = object_types::weapon_rack;
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
	else
		result = object_types::type_none;

	return result;
}

std::string NameToTypeConversion::get_object_name_by_type(object_types type)
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
	case object_types::chest:
		result = "chest";
		break;
	case object_types::lever:
		result = "lever";
		break;
	case object_types::hidden_door:
		result = "hidden_door";
		break;
	case object_types::map:
		result = "map";
		break;
	case object_types::barrel:
		result = "barrel";
		break;
	case object_types::boards_vertical:
		result = "boards_vertical";
		break;
	case object_types::weapon_rack:
		result = "weapon_rack";
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
	default:
		result = "default";
	}

	return result;
}