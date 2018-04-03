#include "collision_system.h"



SDL_Point collision_system::resolve_collisions(ICollidable* character_collision, IMoving* mc, ITerrain* tc)
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
		ltt = static_cast<ITile*>(left_top->get_component(Component::ComponentType::Tile));
	bool ltttrav = false;
	if (ltt)
		ltttrav = ltt->is_traversible;

	if (left_top && !ltttrav)
	{
		Transform* tile_transf = static_cast<Transform*>(left_top->get_component(Component::ComponentType::Transf));
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
			rtt = static_cast<ITile*>(right_top->get_component(Component::ComponentType::Tile));
		bool rtttrav = false;
		if (rtt)
			rtttrav = rtt->is_traversible;

		if (right_top && !rtttrav)
		{
			Transform* tile_transf = static_cast<Transform*>(right_top->get_component(Component::ComponentType::Transf));
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
		top_tilec = static_cast<ITile*>(top->get_component(Component::ComponentType::Tile));
	bool top_trav = false;
	if (top_tilec)
		top_trav = top_tilec->is_traversible;

	if (top && !top_trav)
	{
		Transform* tile_transf = static_cast<Transform*>(top->get_component(Component::ComponentType::Transf));
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
			bottom_tilec = static_cast<ITile*>(bottom->get_component(Component::ComponentType::Tile));
		bool bottom_trav = false;
		if (bottom_tilec)
			bottom_trav = bottom_tilec->is_traversible;

		if (bottom && !bottom_trav)
		{
			Transform* tile_transf = static_cast<Transform*>(bottom->get_component(Component::ComponentType::Transf));
			if (geometry_utilities::has_intersection(desired_rect, tile_transf->position))
			{
				required_delta.y = tile_transf->position.y - (desired_rect.y + desired_rect.h);
				dest_altered_y = true;
			}
		}
	}

	return required_delta;
}

collision_system::collision_system()
{
}


collision_system::~collision_system()
{
}
