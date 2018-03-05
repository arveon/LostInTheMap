#pragma once
#include <string>
#include "Components.h"

struct ui_element_config
{
	std::string name;
	SDL_Point position;
	UI_Element_Type type;
};

struct Button : ui_element_config 
{
	Button() { type = UI_Element_Type::button; }
};
struct Slider : ui_element_config 
{
	int value = 0;
	Slider(int val) { value = val; type = UI_Element_Type::slider_slab; }
};



struct MenuLayout
{
	std::vector<ui_element_config> buttons;
	std::string background_path;
};