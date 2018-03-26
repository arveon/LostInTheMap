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
	mouse_system::change_mouse_icon(mouse_system::mouse_icons::walking, static_cast<IAnimatable*>(mouse->get_component(ComponentType::Animated)), static_cast<IDrawable*>(mouse->get_component(ComponentType::Drawable)));
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

			Transform* tr = static_cast<Transform*>(tile->get_component(ComponentType::Transf));
			IDrawable* dc = static_cast<IDrawable*>(tile->get_component(ComponentType::Drawable));
			
			mc->pathfinder.set_destination({ t_ids.x, t_ids .y});//set pathfinders destination as well
			//calculate and get path from pathfinder
			mc->path = mc->pathfinder.get_path_to({ t_ids.x, t_ids.y});
			
			SDL_Point player_ids = map_system::world_to_tilemap_ids(player->get_origin_in_world(), tc);
			if (!mc->path.empty())
			{
				//set destination mouse click position
				SDL_Point player_sp_or = player->get_sprite_origin();
				mc->final_destination = { mouse_pos.x - player_sp_or.x, mouse_pos.y - player_sp_or.y };
				mc->destination_reached = false;
				//check if point overlaps with any of the objects
				Entity* target = game_flow_normal::get_object_at_point(space, mouse_pos.x, mouse_pos.y);
				if (target != nullptr)
				{
					//if it does, remove last node of path (will finish moving one tile before the object)
					mc->final_destination = { tr->position.x, tr->position.y };
					mc->path.pop_back();
				}
			}
			else if (tile == tc->terrain_tiles[player_ids.y][player_ids.x])
			{
				SDL_Point player_sp_or = player->get_sprite_origin();
				mc->final_destination = {mouse_pos.x - player_sp_or.x, mouse_pos.y - player_sp_or.y };
				mc->destination_reached = false;
			}
			if (mc->path.size() != 0 || (mc->final_destination.x != tr->position.x && mc->final_destination.y != tr->position.y))
				mc->destination_reached = false;
		}
			
		lmb_down_event = false;//clear event flag
	}
	if (lmb_up_event)
	{
		lmb_up_event = false;//clear event flag
	}

	SpaceSystem::update_draw_rects(space);
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
