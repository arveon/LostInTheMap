#include "combat_flow.h"

bool combat_flow::combat_finished = true;
bool combat_flow::initialised = false;
Space combat_flow::combat_space;
std::vector<army_unit> combat_flow::player_army;
std::vector<army_unit> combat_flow::enemy_army;
std::vector<army_unit*> combat_flow::order_of_turns;
int combat_flow::cur_turn;

Entity* combat_flow::mouse;

void combat_flow::init_player_army(std::vector<army_unit> army)
{
	if (player_army.size() == 0)
		combat_flow::player_army = army;
}

void combat_flow::init_enemy_army(std::vector<army_unit> army)
{
	if (enemy_army.size() == 0)
		combat_flow::enemy_army = army;
}

void combat_flow::init_combat_space(Space& game_space)
{
	for (Entity* t : game_space.objects)
	{
		std::string first_letters = t->name.substr(0, 2);
		//if not a combat objects
		if (first_letters.compare("cb") != 0)
		{
			IDrawable* dc = static_cast<IDrawable*>(t->get_component(Component::ComponentType::Drawable));
			if(dc)
				dc->isActive = false;
		}
		else
			combat_space.objects.push_back(t);
	}
	mouse = mouse_system::create_mouse();
	mouse->name = "cb_mouse";
	game_space.objects.push_back(mouse);
	render_system::add_object_to_queue(static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable)));

	//add terrain to renderer
	Entity* cb_terrain = SpaceSystem::find_entity_by_name(combat_space, "cb_terrain");
	ITerrain* terrain = static_cast<ITerrain*>(cb_terrain->get_component(Component::ComponentType::Terrain));
	std::vector<SDL_manager::Tile> tiles;
	for (int i = 0; i < terrain->height; i++)
		for (int j = 0; j < terrain->width; j++)
		{
			Entity* obj = terrain->terrain_tiles[i][j];
			IDrawable* dc = static_cast<IDrawable*>(obj->get_component(Component::ComponentType::Drawable));
			//tiles.push_back({ dc->sprite, dc->draw_rect });
			if (dc)
				dc->id = render_system::add_object_to_queue(dc);

		}

	render_system::unprepare_terrain();

	//disable previous terrain
	ITerrain* tc = SpaceSystem::get_terrain(game_space);
	for(int i =0; i < tc->height; i++)
		for (int j = 0; j < tc->width; j++)
		{
			if (tc->terrain_tiles[i][j])
			{
				IDrawable* dc = static_cast<IDrawable*>(tc->terrain_tiles[i][j]->get_component(Component::ComponentType::Drawable));
				dc->isActive = false;
			}
		}

	//render_system::set_terrain_texture(SDL_manager::create_terrain_texture(tiles, terrain->width*terrain->tile_width, terrain->height * terrain->tile_height));
	//add other elements to renderer
	SpaceSystem::add_space_to_render_queue(combat_space);

	SDL_Point camera_dest = { (terrain->width / 2)*terrain->tile_width, (terrain->height / 2)*terrain->tile_height };
	camera_system::move_camera_to({0,0});
	camera_system::set_camera_zoom(1.93f);

	//init pathfinder
	lee_pathfinder::init_pathfinder(map_system::get_pathfinding_map(terrain), terrain->width, terrain->height);

	combat_flow::compose_turn_orders();
	movement_system::set_movement_finished_callback(&unit_finished_moving);

	combat_space.initialised = true;
	initialised = true;

	//TODO remove this when moving further into combat flow
	/*combat_finished = true;*/
	std::cout << "Combat started" << std::endl;
}



void combat_flow::destroy_combat(Space& game_space)
{
	//logic for destroying all combat structs
	for (int i = (int)game_space.objects.size() - 1; i >= 0; i--)
	{
		Entity* t = game_space.objects.at(i);
		std::string first_letters = t->name.substr(0, 2);
		//remove all of the combat objects from game space and 
		if (first_letters.compare("cb") == 0)
		{
			game_space.objects.erase(game_space.objects.begin() + i);
			//remove all combat stuff from renderer
			IDrawable* dc = static_cast<IDrawable*>(t->get_component(Component::ComponentType::Drawable));
			if(dc)
				render_system::remove_from_queue(dc);
		}
		else
			if (t->is_active)
			{
				IDrawable* dc = static_cast<IDrawable*>(t->get_component(Component::ComponentType::Drawable));
				if (dc)
					dc->isActive = true;
			}
	}

	//destroy the terrain
	Entity* cb_terrain = SpaceSystem::find_entity_by_name(combat_space, "cb_terrain");
	ITerrain* terrain = static_cast<ITerrain*>(cb_terrain->get_component(Component::ComponentType::Terrain));
	for (int i = 0; i < terrain->height; i++)
		for (int j = 0; j < terrain->width; j++)
		{
			Entity* obj = terrain->terrain_tiles[i][j];
			delete obj->transform;
			for (Component* c : obj->components)
				delete c;

			delete obj;
		}

	delete mouse->transform;
	for (Component* c : mouse->components)
		delete c;

	delete mouse;

	SpaceSystem::destroy_space(combat_space, false);

	initialised = false;
	combat_finished = true;
	std::cout << "Combat ended" << std::endl;
	cur_turn = 0;
}

void combat_flow::update(Space & game_space, int dt)
{
	mouse_system::update_mouse_combat(combat_flow::mouse, game_space, order_of_turns.at(cur_turn)->speed, order_of_turns.at(cur_turn)->unit_entity);

	Entity* tr = SpaceSystem::find_entity_by_name(combat_space, "cb_terrain");
	ITerrain* tc = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));
	movement_system::move_characters_tick_combat(combat_space,dt,tc);
}

void combat_flow::mouse_clicked()
{
	//only process mouse input if it was clicked NOT during an enemy turn
	if (!order_of_turns.at(cur_turn)->is_enemy)
	{
		std::cout << cur_turn << " tried to move, health " << order_of_turns.at(cur_turn)->health_of_first << std::endl;
		Entity* tr = SpaceSystem::find_entity_by_name(combat_space, "cb_terrain");
		ITerrain* tc = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));
		IMoving* mc = static_cast<IMoving*>(order_of_turns.at(cur_turn)->unit_entity->get_component(Component::ComponentType::Movement));
		/*if (mc->path.size() != 0 && mc->path.size() <= order_of_turns.at(cur_turn)->speed)
		{*/
		if(mc->destination_reached)
			character_system::set_final_destination_combat(tc, order_of_turns.at(cur_turn)->unit_entity, mouse_system::get_mouse_in_world(combat_flow::mouse), combat_space);
		//}
	}
	else
		unit_finished_turn();//TODO replace with AI decision making


}

void combat_flow::unit_finished_moving(Entity* unit)
{
	unit_finished_turn();//TODO make it so unit_finished_turn() is only called when a unit finished movement
}

void combat_flow::unit_finished_turn()
{
	cur_turn--;
	if(cur_turn >= 0)
		while (order_of_turns.at(cur_turn)->health_of_first <= 0)//will make sure that dead units don't get a turn
		{
			std::cout << "dead" << std::endl;
			cur_turn--;
			if (cur_turn < 0)
				break;
		}
	if (cur_turn < 0)
		combat_round_finished();
	

	//check if there are live units on both sides (winning/losing conditions)
	bool player = false;
	for (army_unit u : player_army)
		if (u.health_of_first > 0)
		{
			player = true;
			break;
		}

	bool enemy = false;
	for (army_unit u : enemy_army)
		if (u.health_of_first > 0)
		{
			enemy = true;
			break;
		}

	if (!player || !enemy)
	{
		combat_finished = true;
	}
}

void combat_flow::combat_round_finished()
{
	cur_turn = order_of_turns.size() - 1;
	while (order_of_turns.at(cur_turn)->health_of_first <= 0)//will make sure that dead units don't get a turn
	{
		std::cout << "dead" << std::endl;
		cur_turn--;
		if (cur_turn < 0)
			break;
	}
}

void combat_flow::compose_turn_orders()
{
	//compose vector of all of the units
	std::vector<army_unit*> all_units;
	for (int i = 0; i < player_army.size(); i++)
		all_units.push_back(&player_army.at(i));
	for (int i = 0; i < enemy_army.size(); i++)
		all_units.push_back(&enemy_army.at(i));

	//compose vector starting slowest
	for (int i = 0; i < all_units.size(); i++)
	{
		army_unit* slowest = all_units.at(i);
		for (int j = i+1; j < all_units.size(); j++)
		{
			army_unit* un = all_units.at(j);
			if (un->speed < slowest->speed)
			{
				all_units.at(i) = un;
				all_units.at(j) = slowest;
				slowest = un;
			}
			else if (un->speed == slowest->speed)
			{
				//make sure that if player and enemy units have same speed, player goes first
				if (un->is_enemy && !slowest->is_enemy)
				{
					all_units.at(j) = slowest;
					all_units.at(i) = un;
					slowest = un;
				}
			}
		}
		order_of_turns.push_back(slowest);
	}
	//start from fastest (at the end of vector)
	cur_turn = order_of_turns.size() - 1;
}

combat_flow::combat_flow()
{
}


combat_flow::~combat_flow()
{
}
