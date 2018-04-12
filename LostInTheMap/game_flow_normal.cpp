#include "game_flow_normal.h"

Entity* game_flow_normal::mouse;
int game_flow_normal::mouse_up_listener_id;
int game_flow_normal::mouse_down_listener_id;
bool game_flow_normal::lmb_down_event;
bool game_flow_normal::lmb_up_event;
int game_flow_normal::r_down_listener_id;
bool game_flow_normal::r_down_event;

void(*game_flow_normal::reload_game)();

void game_flow_normal::init(Space & game_space)
{
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

	//init mouse
	game_flow_normal::mouse = mouse_system::create_mouse();
	game_space.objects.push_back(mouse);
	render_system::add_object_to_queue(static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable)));

	//initialise camera
	Entity* terrain = SpaceSystem::find_entity_by_name(game_space, "terrain");
	ITerrain* tc = static_cast<ITerrain*>(terrain->get_component(Component::ComponentType::Terrain));
	//camera_system::init_camera(tc->tile_width);
	camera_system::set_camera_zoom(2.f);

	SDL_Rect camera_rect_ids = camera_system::get_camera_rect_ids(tc->tile_width);
	lee_pathfinder::set_camera_position(camera_rect_ids.x, camera_rect_ids.y);
	lee_pathfinder::set_camera_dimensions(camera_rect_ids.w, camera_rect_ids.h);
	mouse_system::change_mouse_icon(mouse_system::mouse_icons::walking, static_cast<IAnimatable*>(mouse->get_component(Component::ComponentType::Animated)), static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable)));

	//init player system
	player_system::set_player(SpaceSystem::find_entity_by_name(game_space, "player"));

	game_space.initialised = true;
	game_flow_normal::lmb_down_event = false;
}

void game_flow_normal::update_space(Space & space, int dt)
{
	static int tw = 0;
	mouse_system::update_mouse(game_flow_normal::mouse, space, dialogue_system::dialogue_pending());
	mouse_system::update_mouse_hover(space, game_flow_normal::mouse);
	camera_system::update_camera();
	SpaceSystem::apply_animation_sprite_changes(space);

	
	SpaceSystem::update_draw_rects(space);

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

		if(portrait_dc->sprite == nullptr)
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

		Entity* terrain = SpaceSystem::find_entity_by_name(space, "terrain");
		ITerrain* tr = static_cast<ITerrain*>(terrain->get_component(Component::ComponentType::Terrain));
		movement_system::move_characters_tick(space, dt, tr);
		game_flow_normal::update_pathfinder(space);
	}
	game_flow_normal::handle_mouse_clicks(space);
}

void game_flow_normal::handle_mouse_clicks(Space& space)
{
	//handle mouse events
	if (lmb_down_event)
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
		else
			set_movement(space);

		lmb_down_event = false;//clear event flag
	}
	if (lmb_up_event)
	{
		lmb_up_event = false;//clear event flag
	}

	//FOR TESTING
	if (r_down_event)
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
		director::clear_bound();
		//asset_controller::destroy_terrain_textures();
		lee_pathfinder::destroy_pathfinding();
		reload_game();
		r_down_event = false;
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
		character_system::set_final_destination(tc, player, mouse_pos, space);
	}
}

void game_flow_normal::update_pathfinder(Space& space)
{
	lee_pathfinder::reset_obstructed();
	
	Entity* tr = SpaceSystem::find_entity_by_name(space, "terrain");
	if (!tr)
		return;
	ITerrain* trc = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));
	//set obstructed tiles in pathfinder
	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		Entity* ent = space.objects.at(i);

		SDL_Point t = ent->get_collision_origin_in_world();
		if (t.x == -1 && t.y == -1)
			continue;

		t = map_system::world_to_tilemap_ids(t, trc);
		lee_pathfinder::set_obstructed(t.x, t.y);
	}

	//set camera inside pathfinder
	SDL_Rect camera_rect_ids = camera_system::get_camera_rect_ids(trc->tile_width);
	lee_pathfinder::set_camera_position(camera_rect_ids.x, camera_rect_ids.y);
	lee_pathfinder::set_camera_dimensions(camera_rect_ids.w, camera_rect_ids.h);
}

void game_flow_normal::r_pressed_event()
{
	r_down_event = true;
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