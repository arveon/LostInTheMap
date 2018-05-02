#include "combat_flow.h"

bool combat_flow::combat_finished = true;
bool combat_flow::player_is_winner = false;
bool combat_flow::initialised = false;
Space combat_flow::combat_space;
std::vector<army_unit*> combat_flow::player_army;
std::vector<army_unit*> combat_flow::enemy_army;
std::vector<army_unit*> combat_flow::order_of_turns;
int combat_flow::cur_turn;

Entity* combat_flow::mouse;

void combat_flow::init_player_army(std::vector<army_unit*> army)
{
	if (player_army.size() == 0)
		combat_flow::player_army = army;
}

void combat_flow::init_enemy_army(std::vector<army_unit*> army)
{
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
		{
			IDescriptable* d = static_cast<IDescriptable*>(t->get_component(Component::ComponentType::Description));
			if (d)
				combat_space.objects.push_back(d->description);

			combat_space.objects.push_back(t);
		}
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
	//camera_system::set_camera_zoom(1.f);
	camera_system::set_camera_zoom(1.93f);

	//init pathfinder
	lee_pathfinder::init_pathfinder(map_system::get_pathfinding_map(terrain), terrain->width, terrain->height, false);

	combat_flow::compose_turn_orders();
	movement_system::set_movement_finished_callback(&unit_finished_moving);

	if (order_of_turns.at(cur_turn)->health_of_first != 0)
	{
		IAnimatable* ac = (IAnimatable*)order_of_turns.at(cur_turn)->unit_entity->get_component(Component::ComponentType::Animated);
		animator::start_animation(ac, animations::selected);
	}
	else
		unit_finished_turn();

	combat_space.initialised = true;
	initialised = true;

	srand(time(0));

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
			IDrawable* dc = (IDrawable*)obj->get_component(Component::ComponentType::Drawable);
			if (dc)
				render_system::remove_from_queue(dc);
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
	order_of_turns.clear();
	enemy_army.clear();

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
		Entity* tr = SpaceSystem::find_entity_by_name(combat_space, "cb_terrain");
		ITerrain* tc = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));
		IMoving* mc = static_cast<IMoving*>(order_of_turns.at(cur_turn)->unit_entity->get_component(Component::ComponentType::Movement));

		ICombatUnit* cbu = (ICombatUnit*)order_of_turns.at(cur_turn)->unit_entity->get_component(Component::ComponentType::CombatUnit);
		if (cbu->skipping_turn)
		{
			cbu->skipping_turn = false;
			unit_finished_turn();
		}
		else if (mc->path.size() != 0 && mc->path.size() <= order_of_turns.at(cur_turn)->speed)
		{
			if (mc->destination_reached)
			{
				mc->movement_allowed = true;
				character_system::set_final_destination_combat(tc, order_of_turns.at(cur_turn)->unit_entity, mouse_system::get_mouse_in_world(combat_flow::mouse), combat_space);
				if (mc->movement_allowed)
					mouse_system::change_mouse_icon(
						mouse_system::mouse_icons::normal, 
						static_cast<IAnimatable*>(mouse->get_component(Component::ComponentType::Animated)), 
						static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable)));
				mouse_system::disable_mouse(mouse);
			}
		}

		if (mc->path.size() == 0 && cbu->attacking)
		{
			mouse_system::disable_mouse(mouse);
			unit_attacks(cbu->owner, cbu->attacking);
		}
	}
}

void combat_flow::unit_attacks(Entity* source, Entity* target)
{
	IAnimatable* ac = (IAnimatable*)source->get_component(Component::ComponentType::Animated);
	if(source->transform->position.x < target->transform->position.x)
		animator::start_animation(ac, animations::attacking_left,&attack_animation_finished_callback);
	else
		animator::start_animation(ac, animations::attacking_right, &attack_animation_finished_callback);
}

void combat_flow::attack_animation_finished_callback(Entity* source)
{
	ICombatUnit* cbu1 = (ICombatUnit*)source->get_component(Component::ComponentType::CombatUnit);
	IAnimatable* ac1 = (IAnimatable*)source->get_component(Component::ComponentType::Animated);
	animator::start_animation(ac1, animations::idle, nullptr);


	Entity* target = cbu1->attacking;
	IAnimatable* ac = (IAnimatable*)target->get_component(Component::ComponentType::Animated);
	ICombatUnit* cbu2 = (ICombatUnit*)target->get_component(Component::ComponentType::CombatUnit);

	//deal damage equal to (randomly generated value in range of min and max dmg) * (quantity)
	int gap = (cbu1->unit_stats->max_damage_close - cbu1->unit_stats->min_damage_close);
	float generated = rand()%gap+1;
	int damage_dealt = (cbu1->unit_stats->min_damage_close + generated) * cbu1->unit_stats->quantity;

	//get total health of unit stack
	int total_unit_health = (cbu2->unit_stats->quantity - 1) * cbu2->unit_stats->max_health + cbu2->unit_stats->health_of_first;
	total_unit_health -= damage_dealt;
	if (total_unit_health > 0)
	{
		cbu2->unit_stats->quantity = std::ceil((float)total_unit_health / (float)cbu2->unit_stats->max_health);
		cbu2->unit_stats->health_of_first = total_unit_health % cbu2->unit_stats->max_health;//remainder of rem_health/max_health
		combat_flow::update_quantity_display(target);
	}
	else
	{
		cbu2->unit_stats->quantity = 0;
		cbu2->unit_stats->health_of_first = 0;
		cbu2->dead = true;
		combat_flow::update_quantity_display(target);
	}

	if(cbu2->unit_stats->quantity > 0)
		animator::start_animation(ac, animations::taking_damage, &damaged_animation_finished_callback);
	else
	{
		animator::start_animation(ac, animations::death, &damaged_animation_finished_callback);
		cbu2->unit_stats->health_of_first = 0;
	}
}

void combat_flow::update_quantity_display(Entity* target)
{
	ICombatUnit* cbu2 = (ICombatUnit*)target->get_component(Component::ComponentType::CombatUnit);
	IDescriptable* ddc = (IDescriptable*)target->get_component(Component::ComponentType::Description);

	if (cbu2->dead)
		ddc->description->deactivate();
	
	IDrawable* desc_draw = (IDrawable*)ddc->description->get_component(Component::ComponentType::Drawable);

	ddc->text = std::to_string(cbu2->unit_stats->quantity);
	ddc->rendered_text = asset_controller::get_texture_from_text(ddc->text, UI_text_type::game_ui_small);
	SDL_Rect text_rect = asset_controller::get_texture_size(ddc->rendered_text);

	desc_draw->sprite = asset_controller::get_texture_from_two(ddc->box_background, ddc->rendered_text, ddc->description->transform->position.w, ddc->description->transform->position.h, text_rect);
}

void combat_flow::damaged_animation_finished_callback(Entity* source)
{
	IAnimatable* ac2 = (IAnimatable*)source->get_component(Component::ComponentType::Animated);
	ICombatUnit* cbu2 = (ICombatUnit*)source->get_component(Component::ComponentType::CombatUnit);

	if(!cbu2->dead)
		animator::start_animation(ac2, animations::idle);

	unit_finished_turn();
}

void combat_flow::unit_finished_moving(Entity* unit)
{
	IMoving* mc = static_cast<IMoving*>(unit->get_component(Component::ComponentType::Movement));
	mc->movement_allowed = false;
	ICombatUnit* cbu = static_cast<ICombatUnit*>(unit->get_component(Component::ComponentType::CombatUnit));
	if (cbu->attacking)
		unit_attacks(unit, cbu->attacking);
	else
	{
		IAnimatable* ac = (IAnimatable*)(unit->get_component(Component::ComponentType::Animated));
		animator::start_animation(ac, animations::idle);
		unit_finished_turn();
	}
}

void combat_flow::unit_finished_turn()
{
	if (combat_flow::check_combat_finished())
		return;

	//reset animation just in case
	IAnimatable* ac = (IAnimatable*)order_of_turns.at(cur_turn)->unit_entity->get_component(Component::ComponentType::Animated);
	animator::start_animation(ac, animations::idle);

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

	army_unit* a = order_of_turns.at(cur_turn);
	
	if (a->is_enemy)
	{
		mouse_system::disable_mouse(mouse);
		switch (a->type)
		{
		case character_type::rat:
			ai_system::process_rat_move(a->unit_entity, player_army, SpaceSystem::get_terrain(combat_space));
			break;
		default:
			unit_finished_turn();
		}

		IMoving* mc = (IMoving*)a->unit_entity->get_component(Component::ComponentType::Movement);
		if (mc)
		{
			ICombatUnit* cbu = (ICombatUnit*)a->unit_entity->get_component(Component::ComponentType::CombatUnit);
			if (mc->path.size() == 0 && cbu->attacking)
				unit_attacks(a->unit_entity, cbu->attacking);
		}
		
	}
	else
	{
		IAnimatable* ac = (IAnimatable*)a->unit_entity->get_component(Component::ComponentType::Animated);
		animator::start_animation(ac, animations::selected);
		mouse_system::enable_mouse(mouse);
	}


	std::cout << a->unit_entity->name << std::endl;
}

bool combat_flow::check_combat_finished()
{
	//check if there are live units on both sides (winning/losing conditions)
	bool player = false;
	for (army_unit* u : player_army)
		if (u->health_of_first > 0)
		{
			player = true;
			break;
		}

	bool enemy = false;
	for (army_unit* u : enemy_army)
		if (u->health_of_first > 0)
		{
			enemy = true;
			break;
		}
	if (!player || !enemy)
	{
		if (!player)
			player_is_winner = false;
		else if (!enemy)
			player_is_winner = true;
		combat_finished = true;

		for (army_unit* u : order_of_turns)
			u->health_of_first = u->max_health;
	}
	return combat_finished;
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
	order_of_turns.clear();
	//compose vector of all of the units
	std::vector<army_unit*> all_units;
	for (int i = 0; i < player_army.size(); i++)
		all_units.push_back(player_army.at(i));
	for (int i = 0; i < enemy_army.size(); i++)
		all_units.push_back(enemy_army.at(i));

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
