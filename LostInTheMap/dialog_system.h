#pragma once
#include <vector>
#include "Entity.h"

#include "xml_system.h"

class dialog_system
{
private:
	xml_system::Dialogue cur_dialogue;
public:
	void load_dialogue();

	dialog_system();
	~dialog_system();
};

