#include "game_flow_normal.h"

Entity* game_flow_normal::mouse;
int game_flow_normal::mouse_up_listener_id;
int game_flow_normal::mouse_down_listener_id;
bool game_flow_normal::lmb_down_event;
bool game_flow_normal::lmb_up_event;

void(*game_flow_normal::change_level_callback)(levels level);

void game_flow_normal::init(Space & game_space, void(*change_level_cb)(levels))
{
	game_flow_normal::change_level_callback = change_level_cb;

	//add all of the objects in game space to render queue
	for (unsigned int i = 0; i < game_space.objects.size(); i++)
	{
		Entity* ent = game_space.objects.at(i);
		IDrawable* drawable = static_cast<IDrawable*>(ent->get_component(Component::ComponentType::Drawable));

		//if an object is terrain, loop through all of its tiles and add them to queue
		ITerrain* tr = static_cast<ITerrain*>(ent->get_component(Component::ComponentType::Terrain));
		if (tr)
		{
			for (int i = 0; i < tr->height; i++)
				for (int j = 0; j < tr->width; j++)
				{
					if (!tr->terrain_tiles[i][j])
						continue;
					IDrawable* dc = static_cast<IDrawable*>(tr->terrain_tiles[i][j]->get_component(Component::ComponentType::Drawable));
					render_system::add_object_to_queue(dc);
				}
		}

		if (!drawable)
			continue;
		drawable->id = render_system::add_object_to_queue(drawable);
	}

	//TODO rework this function so that this is used instead of a loop
	//SpaceSystem::add_space_to_render_queue(game_space);


	//init mouse
	game_flow_normal::mouse = mouse_system::create_mouse();
	game_space.objects.push_back(mouse);
	render_system::add_object_to_queue(static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable)));

	//initialise camera
	Entity* terrain = SpaceSystem::find_entity_by_name(game_space, "terrain");
	ITerrain* tc = static_cast<ITerrain*>(terrain->get_component(Component::ComponentType::Terrain));
	//camera_system::init_camera(tc->tile_width);
	camera_system::set_camera_zoom(2.f);

	SDL_Rect camera_rect_ids = camera_system::get_camera_rect_ids(tc->tile_width, tc->tile_height);
	lee_pathfinder::set_camera_position(camera_rect_ids.x, camera_rect_ids.y);
	lee_pathfinder::set_camera_dimensions(camera_rect_ids.w, camera_rect_ids.h);
	mouse_system::change_mouse_icon(mouse_system::mouse_icons::walking, static_cast<IAnimatable*>(mouse->get_component(Component::ComponentType::Animated)), static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable)));

	director::set_space(&game_space);

	render_system::prepare_terrain(tc->width * tc->tile_width, tc->height * tc->tile_height);
	game_space.initialised = true;
	game_flow_normal::lmb_down_event = false;

	//update camera needed here as it sets camera coords to player
	camera_system::update_camera(1);
	director::script_trigger("start.xml");

	//create the fade overlay object
	Entity* overlay = new Entity(entity_type::ui_element, "fade_overlay");
	IDrawable* dc = new IDrawable(overlay, IDrawable::layers::ui);
	dc->draw_rect = { 0,0,0,0 };
	SDL_manager::get_window_size(&dc->draw_rect.w, &dc->draw_rect.h);
	dc->sprite = SDL_manager::create_texture(dc->draw_rect.w, dc->draw_rect.h, {0,0,0,1});
	//dc->sprite = asset_controller::load_texture("assets/graphics/characters/default.png");
	asset_controller::set_texture_alpha(dc->sprite, 0);
	overlay->add_component(dc);
	render_system::add_object_to_queue(dc);
	overlay_system::init_fade(overlay);
	army_system::init_player_army_frames(&game_space);
}

void game_flow_normal::update_space(Space & space, int dt)
{
	static int tw = 0;
	static bool combat_fadein = false;
	if (combat_fadein)
	{
		overlay_system::set_fade_in(500, 100, nullptr);
		combat_fadein = false;
	}

	animator::update(space, dt);
	animator::apply_animation_sprite_changes(space);
	SpaceSystem::update_draw_rects(space);

	if (!overlay_system::is_fade_done())
		overlay_system::update(dt);

	if (combat_flow::is_in_combat())
	{
		if (!combat_flow::is_initialised())
			combat_flow::init_combat_space(space);
		combat_flow::update(space, dt);
		update_pathfinder(combat_flow::combat_space);
	}
	else if (!combat_flow::is_in_combat() && combat_flow::is_initialised())
	{
		apply_combat_results(space);
		render_system::add_object_to_queue((IDrawable*)overlay_system::get_fade()->get_component(Component::ComponentType::Drawable));
		combat_fadein = true;
	}
	else
	{
		mouse_system::update_mouse(game_flow_normal::mouse, space, dialogue_system::dialogue_pending(), script_system::is_player_blocked());
		camera_system::update_camera(dt);

		if (dialogue_system::dialogue_pending())
		{
			dialogue_system::update(dt);
			Entity* bg = SpaceSystem::find_entity_by_name(space, "dialogue_panel");
			Entity* portrait = SpaceSystem::find_entity_by_name(space, "dialogue_portrait");
			Entity* text = SpaceSystem::find_entity_by_name(space, "dialogue_text");
			IDrawable* bg_dc = static_cast<IDrawable*>(bg->get_component(Component::ComponentType::Drawable));
			bg_dc->isActive = true;
			IDrawable* portrait_dc = static_cast<IDrawable*>(portrait->get_component(Component::ComponentType::Drawable));
			portrait_dc->isActive = true;
			IDrawable* text_dc = static_cast<IDrawable*>(text->get_component(Component::ComponentType::Drawable));
			text_dc->isActive = true;

			if (portrait_dc->sprite == nullptr)
				portrait_dc->sprite = dialogue_system::get_cur_portrait();

			text_dc->sprite = dialogue_system::get_cur_line_sprite();
			SDL_Rect size = asset_controller::get_texture_size(text_dc->sprite);
			text_dc->draw_rect.w = size.w;
			text_dc->draw_rect.h = size.h;
		}
		else
		{
			Entity* bg = SpaceSystem::find_entity_by_name(space, "dialogue_panel");
			Entity* portrait = SpaceSystem::find_entity_by_name(space, "dialogue_portrait");
			Entity* text = SpaceSystem::find_entity_by_name(space, "dialogue_text");
			IDrawable* bg_dc = static_cast<IDrawable*>(bg->get_component(Component::ComponentType::Drawable));
			bg_dc->isActive = false;
			IDrawable* portrait_dc = static_cast<IDrawable*>(portrait->get_component(Component::ComponentType::Drawable));
			portrait_dc->sprite = nullptr;
			portrait_dc->isActive = false;
			IDrawable* text_dc = static_cast<IDrawable*>(text->get_component(Component::ComponentType::Drawable));
			text_dc->isActive = false;
			game_flow_normal::update_pathfinder(space);
		}
		Entity* terrain = SpaceSystem::find_entity_by_name(space, "terrain");
		ITerrain* tr = static_cast<ITerrain*>(terrain->get_component(Component::ComponentType::Terrain));
		movement_system::move_characters_tick(space, dt, tr);

		if (script_system::is_script_going())
			script_system::update(dt);

		if (director::is_level_switch_pending())
		{
			change_level_callback(director::target_level);
		}
	}
	game_flow_normal::handle_mouse_clicks(space);
}


void game_flow_normal::apply_combat_results(Space& space)
{
	if (!combat_flow::player_is_winner)
	{//if combat lost
		army_system::set_player_army(xml_system::load_army("giovanni.xml", director::cur_level, false));
		change_level_callback(director::cur_level);
	}
	else
	{//if combat won
		script_system::action_over(SpaceSystem::find_entity_by_name(space, "player"));
		combat_flow::destroy_combat(space);

		render_system::flush_queues();

		//return cursor to render queue as it was swapped over by cursor from combat flow
		render_system::add_object_to_queue(static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable)));

		Entity* terrain = SpaceSystem::find_entity_by_name(space, "terrain");
		ITerrain* tc = static_cast<ITerrain*>(terrain->get_component(Component::ComponentType::Terrain));

		SpaceSystem::add_space_to_render_queue(space);
		for (int i = 0; i < tc->height; i++)
			for (int j = 0; j < tc->width; j++)
			{
				if (tc->terrain_tiles[i][j])
				{
					IDrawable* dc = (IDrawable*)tc->terrain_tiles[i][j]->get_component(Component::ComponentType::Drawable);
					if (dc)
					{
						dc->isActive = true;
						render_system::add_object_to_queue(dc);
					}
				}
			}

		render_system::prepare_terrain(tc->width*tc->tile_width, tc->height*tc->tile_height);

		camera_system::set_camera_zoom(2.f);
		Entity* player = SpaceSystem::find_entity_by_name(space, "player");
		camera_system::set_camera_target(player);
		mouse->activate();

		IInteractionSource* p_ics = (IInteractionSource*)player->get_component(Component::ComponentType::InteractionSource);
		p_ics->interaction_target->deactivate();
		//init pathfinder
		lee_pathfinder::init_pathfinder(map_system::get_pathfinding_map(tc), tc->width, tc->height);
		SDL_Rect camera_rect_ids = camera_system::get_camera_rect_ids(tc->tile_width, tc->tile_height);
		lee_pathfinder::set_camera_position(camera_rect_ids.x, camera_rect_ids.y);
		lee_pathfinder::set_camera_dimensions(camera_rect_ids.w, camera_rect_ids.h);
	}
	army_system::update_unit_frames();
}

void game_flow_normal::handle_mouse_clicks(Space& space)
{
	//handle mouse events
	if (lmb_down_event)
	{
		if (!combat_flow::is_in_combat())
		{
			if (dialogue_system::dialogue_pending())
			{
				Entity* text = SpaceSystem::find_entity_by_name(space, "dialogue_text");
				IDrawable* text_dc = static_cast<IDrawable*>(text->get_component(Component::ComponentType::Drawable));

				if (dialogue_system::is_line_done())
				{
					Entity* portrait = SpaceSystem::find_entity_by_name(space, "dialogue_portrait");
					IDrawable* portrait_dc = static_cast<IDrawable*>(portrait->get_component(Component::ComponentType::Drawable));

					dialogue_system::next_line();
					portrait_dc->sprite = dialogue_system::get_cur_portrait();
				}
				else
					dialogue_system::finish_line();
				//std::cout << dialogue_system::get_cur_line() << std::endl;
				text_dc->sprite = dialogue_system::get_cur_line_sprite();
				SDL_Rect size = asset_controller::get_texture_size(text_dc->sprite);
				text_dc->draw_rect.w = size.w;
				text_dc->draw_rect.h = size.h;
			}
			else if (!script_system::is_player_blocked())
				set_movement(space);
		}
		else
		{
			combat_flow::mouse_clicked();
		}

		lmb_down_event = false;//clear event flag
	}
	if (lmb_up_event)
	{
		lmb_up_event = false;//clear event flag
	}
}

void game_flow_normal::set_movement(Space& space)
{
	SDL_Point mouse_pos = mouse_system::get_mouse_in_world(game_flow_normal::mouse);

	//check if there is a tile
	Entity* terrain = SpaceSystem::find_entity_by_name(space, "terrain");
	ITerrain* tc = static_cast<ITerrain*>(terrain->get_component(Component::ComponentType::Terrain));
	if (!tc)
		return;
	Entity* tile = map_system::get_tile_at(terrain, mouse_pos);

	//set players destination to tile if tile exists
	if (tile != nullptr)
	{
		ITile* tilec = static_cast<ITile*>(tile->get_component(Component::ComponentType::Tile));

		//get player and set his destination to mouse click position
		Entity* player = SpaceSystem::find_entity_by_name(space, "player");
		Entity* mouse = SpaceSystem::find_entity_by_name(space, "mouse");
		character_system::set_final_destination(tc, player, mouse_pos, space);
	}
}

void game_flow_normal::update_pathfinder(Space& space)
{
	lee_pathfinder::reset_obstructed();

	Entity* tr = SpaceSystem::find_entity_by_name(space, "terrain");
	if (!tr)
		tr = SpaceSystem::find_entity_by_name(space, "cb_terrain");
	if (!tr)
		return;
	ITerrain* trc = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));
	//set obstructed tiles in pathfinder
	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		Entity* ent = space.objects.at(i);

		ICombatUnit* cbu = (ICombatUnit*)ent->get_component(Component::ComponentType::CombatUnit);
		if (cbu)
			if (cbu->dead)
				continue;

		if (!ent->get_component(Component::ComponentType::Collision) || !ent->is_active)
			continue;

		SDL_Point t = ent->get_origin_in_world();
		if (t.x == -1 && t.y == -1)
			continue;

		SDL_Point b = map_system::world_to_tilemap_ids(t, trc);
		lee_pathfinder::set_obstructed(b.x, b.y);
	}

	//set camera inside pathfinder
	SDL_Rect camera_rect_ids = camera_system::get_camera_rect_ids(trc->tile_width, trc->tile_height);
	lee_pathfinder::set_camera_position(camera_rect_ids.x, camera_rect_ids.y);
	lee_pathfinder::set_camera_dimensions(camera_rect_ids.w, camera_rect_ids.h);
}

void game_flow_normal::clear_all_systems(Space& space)
{
	Entity* terrain = SpaceSystem::find_entity_by_name(space, "terrain");
	ITerrain* tc = static_cast<ITerrain*>(terrain->get_component(Component::ComponentType::Terrain));
	for (int i = 0; i < tc->height; i++)
	{
		for (int j = 0; j < tc->width; j++)
		{
			Entity* tile = tc->terrain_tiles[i][j];
			if (tile != nullptr)
			{
				delete tile->transform;
				for (unsigned int k = tile->components.size() - 1; k >= 0; k--)
				{
					if (k > tile->components.size())
						break;
					Component* temp = tile->components.at(k);
					tile->components.erase(tile->components.begin() + k);
					delete temp;
				}
				delete tile;
			}
		}
	}

	character_system::clear_characters();
	director::reset_director();
	asset_controller::clear_stored_textures();
	lee_pathfinder::destroy_pathfinding();
	if (combat_flow::is_initialised())
		combat_flow::destroy_combat(space);
	army_system::reset_army_system();
}

void game_flow_normal::mouse_down_event()
{
	lmb_down_event = true;
}

void game_flow_normal::mouse_up_event()
{
	lmb_up_event = true;
}

game_flow_normal::game_flow_normal()
{
}


game_flow_normal::~game_flow_normal()
{
}