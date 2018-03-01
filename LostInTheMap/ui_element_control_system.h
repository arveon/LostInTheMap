#pragma once
#include "Entity.h"


class ui_element_control_system
{
public:
	ui_element_control_system();
	~ui_element_control_system();

	virtual void on_click(Entity* element) = 0;
	virtual void on_hover(Entity* element) = 0;
	virtual void on_release(Entity* element) = 0;
	
};

