#include "game_flow_normal.h"

Entity* game_flow_normal::mouse;
int game_flow_normal::mouse_up_listener_id;
int game_flow_normal::mouse_down_listener_id;
bool game_flow_normal::lmb_down_event;
bool game_flow_normal::lmb_up_event;

void game_flow_normal::init(Space & game_space)
{
	//add all of the objects in game space to render queue
	for (unsigned int i = 0; i < game_space.objects.size(); i++)
	{
		Entity* ent = game_space.objects.at(i);
		IDrawable* drawable = static_cast<IDrawable*>(ent->get_component(ComponentType::Drawable));

		//if an object is terrain, loop through all of its tiles and add them to queue
		ITerrain* tr = static_cast<ITerrain*>(ent->get_component(ComponentType::Terrain));
		if (tr)
		{
			for (int i = 0; i < tr->height; i++)
				for (int j = 0; j < tr->width; j++)
				{
					if (!tr->terrain_tiles[i][j])
						continue;
					IDrawable* dc = static_cast<IDrawable*>(tr->terrain_tiles[i][j]->get_component(ComponentType::Drawable));
					render_system::add_object_to_queue(dc);
				}
		}

		if (!drawable)
			continue;
		render_system::add_object_to_queue(drawable);
	}
	game_flow_normal::mouse = mouse_system::create_mouse();
	game_space.objects.push_back(mouse);
	render_system::add_object_to_queue(static_cast<IDrawable*>(mouse->get_component(ComponentType::Drawable)));

	camera_system::set_camera_zoom(2.f);
	//mouse_system::change_mouse_icon(mouse_system::mouse_icons::walking, static_cast<IAnimatable*>(mouse->get_component(ComponentType::Animated)), static_cast<IDrawable*>(mouse->get_component(ComponentType::Drawable)));
	game_space.initialised = true;
	game_flow_normal::lmb_down_event = false;

}

void game_flow_normal::update_space(Space & space, int dt)
{
	static int tw = 0;
	mouse_system::update_mouse(game_flow_normal::mouse);
	camera_system::update_camera();
	SpaceSystem::apply_animation_sprite_changes(space);

	Entity* terrain = SpaceSystem::find_entity_by_name(space, "terrain");
	ITerrain* tr = static_cast<ITerrain*>(terrain->get_component(ComponentType::Terrain));
	movement_system::move_characters_tick(space, dt, tr);

	game_flow_normal::handle_mouse_events(space);
	SpaceSystem::update_draw_rects(space);
}

void game_flow_normal::handle_mouse_events(Space& space)
{
	//handle mouse events
	if (lmb_down_event)
	{
		IMouse* ms = static_cast<IMouse*>(game_flow_normal::mouse->get_component(ComponentType::Mouse));

		//get mouse screen space
		SDL_Point mouse_pos = { input_system::mouse.x, input_system::mouse.y };
		//get mouse world space
		mouse_pos = camera_system::screen_to_world_space(mouse_pos);


		//see if the tilemap tile exists
		Entity* trn = SpaceSystem::find_entity_by_name(space, "terrain");
		ITerrain* tc = static_cast<ITerrain*>(trn->get_component(ComponentType::Terrain));
		if (!tc)
			return;

		SDL_Point t_ids = map_system::world_to_tilemap_ids(mouse_pos, tc);

		Entity* tile = tc->terrain_tiles[t_ids.y][t_ids.x];
		//set players destination to tile if tile exists
		if (tile != nullptr)
		{
			//get player transform and move component
			Entity* player = SpaceSystem::find_entity_by_name(space, "player");
			IMoving* mc = static_cast<IMoving*>(player->get_component(ComponentType::Movement));
			ICollidable* colc = static_cast<ICollidable*>(player->get_component(ComponentType::Collision));

			Transform* tr = static_cast<Transform*>(tile->get_component(ComponentType::Transf));
			IDrawable* dc = static_cast<IDrawable*>(tile->get_component(ComponentType::Drawable));

			mc->pathfinder.set_destination({ t_ids.x, t_ids.y });//set pathfinders destination as well
																 //calculate and get path from pathfinder
			mc->path = mc->pathfinder.get_path_to({ t_ids.x, t_ids.y });
			ITile* tilec = static_cast<ITile*>(tile->get_component(ComponentType::Tile));

			SDL_Point player_ids = map_system::world_to_tilemap_ids(player->get_origin_in_world(), tc);
			if (!mc->path.empty())
			{
				//set destination mouse click position
				SDL_Point player_sp_or = player->get_sprite_origin();
				mc->final_destination = { mouse_pos.x - player_sp_or.x, mouse_pos.y - player_sp_or.y };
				SDL_Point desired_point = game_flow_normal::resolve_collisions(colc, mc, tc);
				mc->final_destination.x += desired_point.x;
				mc->final_destination.y += desired_point.y;
				mc->destination_reached = false;
				//check if point overlaps with any of the objects
				Entity* target = game_flow_normal::get_object_at_point(space, mouse_pos.x, mouse_pos.y);

				//breaks everything
				//if (target != nullptr)
				//{
				//	//if it does, remove last node of path (will finish moving one tile before the object)
				//	mc->final_destination = { tr->position.x, tr->position.y };
				//	mc->path.pop_back();
				//}
			}
			else if (tile == tc->terrain_tiles[player_ids.y][player_ids.x] && tilec->is_traversible)
			{
				SDL_Point player_sp_or = player->get_sprite_origin();
				mc->final_destination = { mouse_pos.x - player_sp_or.x, mouse_pos.y - player_sp_or.y };

				SDL_Point desired_point = game_flow_normal::resolve_collisions(colc, mc, tc);
				mc->final_destination.x += desired_point.x;
				mc->final_destination.y += desired_point.y;

				mc->destination_reached = false;
			}
		}

		lmb_down_event = false;//clear event flag
	}
	if (lmb_up_event)
	{
		lmb_up_event = false;//clear event flag
	}
}

Entity* game_flow_normal::get_object_at_point(Space& space, int x, int y)
{
	Entity* ent = nullptr;

	for (unsigned int i = 0; i < space.objects.size(); i++)
	{
		Entity* temp = space.objects.at(i);
		Transform* tf = static_cast<Transform*>(temp->get_component(ComponentType::Transf));
		if (!tf)
			continue;
		SDL_Rect rect = tf->position;
		if (geometry_utilities::is_point_in_rect(x, y, rect))
		{
			ent = temp;
			break;
		}
	}

	return ent;
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

SDL_Point game_flow_normal::resolve_collisions(ICollidable* character_collision, IMoving* mc, ITerrain* tc)
{
	bool dest_altered_x = false;
	bool dest_altered_y = false;
	SDL_Point required_delta = { 0,0 };

	//take into account collision rectangle
	//calculate collision rectangle for final destination
	SDL_Rect desired_rect = character_collision->collision_rect;
	desired_rect.x += mc->final_destination.x;
	desired_rect.y += mc->final_destination.y;
	//check if it intersects with any non-traversible tiles on X axis


	//LEFT
	SDL_Point tile_id = map_system::world_to_tilemap_ids({ desired_rect.x + desired_rect.w / 2, desired_rect.y + desired_rect.h / 2 }, tc);
	Entity* left_top = tc->terrain_tiles[tile_id.y][tile_id.x - 1];

	ITile* ltt = nullptr;
	if (left_top)
		ltt = static_cast<ITile*>(left_top->get_component(ComponentType::Tile));
	bool ltttrav = false;
	if (ltt)
		ltttrav = ltt->is_traversible;

	if (left_top && !ltttrav)
	{
		Transform* tile_transf = static_cast<Transform*>(left_top->get_component(ComponentType::Transf));
		if (geometry_utilities::has_intersection(desired_rect, tile_transf->position))
		{
			required_delta.x = (tile_transf->position.x + tile_transf->position.w) - desired_rect.x;
			dest_altered_x = true;
		}
	}

	//only look at collisions on right, if no collisions on left happened
	if (!dest_altered_x)
	{
		//RIGHT
		Entity* right_top = tc->terrain_tiles[tile_id.y][tile_id.x + 1];

		ITile* rtt = nullptr;
		if (right_top)
			rtt = static_cast<ITile*>(right_top->get_component(ComponentType::Tile));
		bool rtttrav = false;
		if (rtt)
			rtttrav = rtt->is_traversible;

		if (right_top && !rtttrav)
		{
			Transform* tile_transf = static_cast<Transform*>(right_top->get_component(ComponentType::Transf));
			if (geometry_utilities::has_intersection(desired_rect, tile_transf->position))
			{
				required_delta.x = tile_transf->position.x - (desired_rect.x + desired_rect.w);
				dest_altered_x = true;
			}
		}
	}

	//TOP
	SDL_Point top_tile_id = map_system::world_to_tilemap_ids({ desired_rect.x + desired_rect.w / 2, desired_rect.y }, tc);
	Entity* top = tc->terrain_tiles[top_tile_id.y][top_tile_id.x];//tile above
	//get whether top tile is traversible or not
	ITile* top_tilec = nullptr;
	if (top)
		top_tilec = static_cast<ITile*>(top->get_component(ComponentType::Tile));
	bool top_trav = false;
	if (top_tilec)
		top_trav = top_tilec->is_traversible;

	if (top && !top_trav)
	{
		Transform* tile_transf = static_cast<Transform*>(top->get_component(ComponentType::Transf));
		if (geometry_utilities::has_intersection(desired_rect, tile_transf->position))
		{
			required_delta.y = (tile_transf->position.y + tile_transf->position.h) - desired_rect.y;
			dest_altered_y = true;
		}
	}

	if (!dest_altered_y)
	{
		//BOTTOM
		SDL_Point bottom_tile_id = map_system::world_to_tilemap_ids({ desired_rect.x + desired_rect.w / 2, desired_rect.y + desired_rect.h }, tc);
		Entity* bottom = tc->terrain_tiles[bottom_tile_id.y][bottom_tile_id.x];//tile above
																	  //get whether top tile is traversible or not
		ITile* bottom_tilec = nullptr;
		if (bottom)
			bottom_tilec = static_cast<ITile*>(bottom->get_component(ComponentType::Tile));
		bool bottom_trav = false;
		if (bottom_tilec)
			bottom_trav = bottom_tilec->is_traversible;

		if (bottom && !bottom_trav)
		{
			Transform* tile_transf = static_cast<Transform*>(bottom->get_component(ComponentType::Transf));
			if (geometry_utilities::has_intersection(desired_rect, tile_transf->position))
			{
				required_delta.y = tile_transf->position.y - (desired_rect.y + desired_rect.h);
				dest_altered_y = true;
			}
		}
	}

	return required_delta;
}
