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
		
		//get player
		Entity* player = SpaceSystem::find_entity_by_name(space, "player");

		//see if the tilemap tile exists
		Entity* trn = SpaceSystem::find_entity_by_name(space, "terrain");
		ITerrain* tc = static_cast<ITerrain*>(trn->get_component(ComponentType::Terrain));
		if (!tc)
			return;
		int t_x = 0, t_y = 0;
		t_x = std::floor(mouse_pos.x / tc->tile_width);
		t_y = std::floor(mouse_pos.y / tc->tile_width);

		Entity* tile = tc->terrain_tiles[t_y][t_x];
		//set players destination to tile if tile exists
		if (tile != nullptr)
		{
			Transform* tr = static_cast<Transform*>(tile->get_component(ComponentType::Transf));
			IMoving* mc = static_cast<IMoving*>(player->get_component(ComponentType::Movement));
			mc->final_destination = { tr->position.x + tr->position.w / 2, tr->position.y + tr->position.h / 2 };//set destination to center of tile
			mc->pathfinder.set_destination({ t_x, t_y });//set pathfinders destination as well
														 //tell pathfinder to calculate path
			mc->path = mc->pathfinder.get_path_to({t_x, t_y});

			//check if point overlaps with any of the objects
			Entity* target = game_flow_normal::get_object_at_point(space, mouse_pos.x, mouse_pos.y);
			if (target != nullptr)
			{
				//if it does, remove last node of path (will finish moving one tile before the object)
				if(!mc->path.empty())
					mc->path.pop_back();
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
