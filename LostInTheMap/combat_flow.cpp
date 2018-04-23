#include "combat_flow.h"

bool combat_flow::combat_finished = true;
bool combat_flow::initialised = false;
Space combat_flow::combat_space;
std::vector<army_unit> combat_flow::player_army;

void combat_flow::init_player_army(std::vector<army_unit> army)
{
	if (player_army.size() == 0)
		combat_flow::player_army = army;
}

void combat_flow::init_combat_space(Space& game_space)
{
	for (Entity* t : game_space.objects)
	{
		std::string first_letters = t->name.substr(0, 2);
		//if not a combat objects
		if (first_letters.compare("cb") != 0)
			continue;

		combat_space.objects.push_back(t);
	}
	combat_space.initialised = true;
	initialised = true;
}

void combat_flow::destroy_combat(Space& game_space)
{
	//logic for destroying all combat structs
	for (int i = (int)game_space.objects.size(); i > 0; i--)
	{
		Entity* t = game_space.objects.at(i);
		std::string first_letters = t->name.substr(0, 2);
		//remove all of the combat objects from game space
		if (first_letters.compare("cb") == 0)
			game_space.objects.erase(game_space.objects.begin()+i);

		//remove all combat stuff from renderer
		IDrawable* dc = static_cast<IDrawable*>(t->get_component(Component::ComponentType::Drawable));
		if (dc)
			render_system::remove_from_queue(dc->id, dc->layer);


	}

	initialised = false;
	combat_finished = true;
}

combat_flow::combat_flow()
{
}


combat_flow::~combat_flow()
{
}
