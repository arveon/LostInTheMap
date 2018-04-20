#include "map_system.h"

int map_system::tile_width;
void map_system::init_terrain_map(int ** tile_ids, Entity* tilemap)
{
	ITerrain* tc = static_cast<ITerrain*>(tilemap->get_component(Component::ComponentType::Terrain));

	int w = tc->width;
	int h = tc->height;
	int t_w = tc->tile_width;
	map_system::tile_width = tc->tile_width;

	//initialise all possible tiles in the map to a nullptr
	tc->terrain_tiles = new Entity**[h];
	for (int y = 0; y < h; y++)
	{
		tc->terrain_tiles[y] = new Entity*[w];
		for (int x = 0; x < w; x++)
		{
			tc->terrain_tiles[y][x] = nullptr;
		}
	}

	//loop through all of the tiles, create entities for them (with names representing the coordinate in the grid) and store tile texture id's in draw component id field
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			//don't create tile objects for empty tiles
			if (tile_ids[y][x] == -1)
			{
				tc->terrain_tiles[y][x] = nullptr;
				continue;
			}
			Entity* tile = new Entity(entity_type::game_object, std::to_string(x) + "_" + std::to_string(y));
			IDrawable* dc = new IDrawable(tile, IDrawable::layers::terrain);
			tile->add_component(dc);
			dc->id = tile_ids[y][x];
			Transform* transf_c = new Transform(tile);
			tile->add_component(transf_c);
			tc->terrain_tiles[y][x] = tile;
			ITile* tlc = new ITile(tile, x, y, true);
			tile->add_component(tlc);

		}
	}

	//display all tile texture id's for testing purposes
	/*for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			IDrawable* dc = static_cast<IDrawable*>(tc->terrain_tiles[y][x]->get_component(ComponentType::Drawable));
			std::cout << dc->id << " ";
		}
		std::cout << std::endl;
	}*/
}

//function adds collision components to entities in tilemap that are collidable
void map_system::init_terrain_collisions(int ** collision_map, Entity * tilemap)
{
	ITerrain* tr = static_cast<ITerrain*>(tilemap->get_component(Component::ComponentType::Terrain));
	for (int i = 0; i < tr->height; i++)
	{
		for (int j = 0; j < tr->width; j++)
		{
			if (tr->terrain_tiles[i][j] == nullptr)
				continue;
			Transform* transform = static_cast<Transform*>(tr->terrain_tiles[i][j]->get_component(Component::ComponentType::Transf));
			transform->position = {
				static_cast<int>(j*tr->tile_width),
				static_cast<int>(i*tr->tile_width),
				static_cast<int>(tr->tile_width),
				static_cast<int>(tr->tile_width)
			};
			transform->origin.x = transform->position.w / 2;
			transform->origin.y = transform->position.h / 2;
			ITile* tc = static_cast<ITile*>(tr->terrain_tiles[i][j]->get_component(Component::ComponentType::Tile));
			if (collision_map[i][j] == 1)
				tc->is_traversible = false;
			else
				tc->is_traversible = true;
		}
	}

	//display the collision map
	for (int y = 0; y < tr->height; y++)
	{
		for (int x = 0; x < tr->width; x++)
		{
			if (tr->terrain_tiles[y][x] == nullptr)
			{
				//std::cout << "1";
				continue;
			}
			ITile* dc = static_cast<ITile*>(tr->terrain_tiles[y][x]->get_component(Component::ComponentType::Tile));
			/*if (dc->is_traversible)
				std::cout << "0";
			else
				std::cout << "1";*/
		}
		//std::cout << std::endl;
	}
}

std::vector<Entity*> map_system::init_triggers(Actor ** trigger_map, ITerrain* tr)
{
	std::vector<Entity*> result;
	static int counter = 0;
	for (int i = 0; i < tr->height; i++)
	{
		for (int j = 0; j < tr->width; j++)
		{
			if (trigger_map[i][j].value == 4)
			{
				Entity* trigger = new Entity(entity_type::trigger, "trigger" + std::to_string(counter));
				counter++;
				Transform* tf = new Transform(trigger);
				tf->position = {
					static_cast<int>(j*tr->tile_width),
					static_cast<int>(i*tr->tile_width),
					static_cast<int>(tr->tile_width),
					static_cast<int>(tr->tile_width)
				};
				trigger->add_component(tf);

				IInteractionSource* is = new IInteractionSource(trigger);
				is->has_triggered = false;
				is->script_attached = trigger_map[i][j].script;
				is->interaction_trigger = &director::script_trigger;
				trigger->add_component(is);

				result.push_back(trigger);
			}


		}
	}


	return result;
}

std::vector<Entity*> map_system::init_objects(Actor ** objects_map, ITerrain* tr)
{
	std::vector<Entity*> result;
	static int counter = 0;
	for (int i = 0; i < tr->height; i++)
	{
		for (int j = 0; j < tr->width; j++)
		{
			if (objects_map[i][j].value == 6 || objects_map[i][j].value == 5)
			{
				Entity* obj = new Entity(entity_type::game_object, "object" + std::to_string(counter));
				counter++;
				Transform* tf = new Transform(obj);
				tf->position = {
					static_cast<int>(j*tr->tile_width),
					static_cast<int>(i*tr->tile_width),
					static_cast<int>(tr->tile_width),
					static_cast<int>(tr->tile_width)
				};
				obj->add_component(tf);

				ICollidable* collider = new ICollidable(obj);
				IDrawable* dc = new IDrawable(obj, IDrawable::layers::surface);
				dc->draw_rect = tf->position;

				object_types type = xml_system::get_object_type_by_name(objects_map[i][j].type);
				tf->origin = { tf->position.w / 2, tf->position.h - 1 };
				
				IInteractionSource* i_obj = new IInteractionSource(obj, type);
				switch (type)
				{
				case object_types::juji_house_1:
				case object_types::juji_house_2:
				{
					dc->draw_rect.w = 128;
					dc->draw_rect.h = 96;
					
					collider->collidable = true;
					collider->collision_rect.w = 90;
					collider->collision_rect.h = 32;
					break;
				}
				case object_types::lever:
				case object_types::pickup_blowgun:
				case object_types::pickup_gold_pile:
				case object_types::pickup_sling:
				case object_types::pickup_spear:
				case object_types::pickup_sword:
				{
					//init collider
					collider->collidable = true;
					collider->collision_rect.w = tf->position.w;
					collider->collision_rect.h = tf->position.h;

					i_obj->interaction_trigger = &director::script_trigger;
					i_obj->script_attached = objects_map[i][j].script;
					break;
				}
				case object_types::barrel:
				case object_types::weapon_rack:
				case object_types::boards_vertical:
				{
					dc->draw_rect.w = 32;
					dc->draw_rect.h = 64;

					collider->collidable = true;
					collider->collision_rect.w = tf->position.w;
					collider->collision_rect.h = tf->position.h;

					i_obj->interaction_trigger = &director::script_trigger;
					i_obj->script_attached = objects_map[i][j].script;
					break;
				}
				
				case object_types::boards_horizontal:
				{
					dc->draw_rect.w = 96;
					dc->draw_rect.h = 32;

					collider->collidable = true;
					collider->collision_rect.w = 96;
					collider->collision_rect.h = tf->position.h;
					
					i_obj->interaction_trigger = &director::script_trigger;
					i_obj->script_attached = objects_map[i][j].script;
					break;
				}
				case object_types::table:
				case object_types::alchemy_table:
				case object_types::chest:
				{
					dc->draw_rect.w = 64;
					dc->draw_rect.h = 64;

					collider->collidable = true;
					collider->collision_rect.w = tf->position.w;
					collider->collision_rect.h = tf->position.h;

					i_obj->interaction_trigger = &director::script_trigger;
					i_obj->script_attached = objects_map[i][j].script;
					break;
				}
				case object_types::hidden_door:
				{
					dc->draw_rect.w = 64;
					dc->draw_rect.h = 32;
					tf->position.w = 64;

					collider->collidable = true;
					collider->collision_rect.w = tf->position.w;
					collider->collision_rect.h = tf->position.h;

					i_obj->interaction_trigger = &director::script_trigger;
					i_obj->script_attached = objects_map[i][j].script;
					break;
				}


				}
				dc->sprite_origin = {dc->draw_rect.w / 2, dc->draw_rect.h};

				obj->add_component(dc);
				obj->add_component(i_obj);
				obj->add_component(collider);

				result.push_back(obj);
			}


		}
	}
	return result;
}

int ** map_system::get_pathfinding_map(ITerrain * tilemap)
{
	int** map;

	//initialise array
	map = new int*[tilemap->height];
	for (int i = 0; i < tilemap->height; i++)
		map[i] = new int[tilemap->width];

	//set up the array
	for (int i = 0; i < tilemap->height; i++)
		for (int j = 0; j < tilemap->width; j++)
		{
			if (tilemap->terrain_tiles[i][j] == nullptr)
			{
				map[i][j] = 1;
				continue;
			}
			ITile* tile = static_cast<ITile*>(tilemap->terrain_tiles[i][j]->get_component(Component::ComponentType::Tile));
			if (tile->is_traversible)
				map[i][j] = 0;
			else
				map[i][j] = 1;
		}

	return map;
}

SDL_Point map_system::world_to_tilemap_ids(SDL_Point world_coords, ITerrain* tilemap)
{
	SDL_Point result;
	result.x = (int)std::floor(world_coords.x / tilemap->tile_width);
	result.y = (int)std::floor(world_coords.y / tilemap->tile_width);
	return result;
}

SDL_Point map_system::tilemap_ids_to_world(SDL_Point ids, ITerrain * tilemap)
{
	SDL_Point result = ids;

	result.x = result.x * tilemap->tile_width;
	result.y = result.y * tilemap->tile_width;

	return result;
}

Entity* map_system::get_tile_at(Entity* terrain, SDL_Point world_coords)
{
	Entity* tile = nullptr;
	if (!terrain)
		return tile;

	//see if the tilemap tile exists
	ITerrain* tc = static_cast<ITerrain*>(terrain->get_component(Component::ComponentType::Terrain));
	if (!tc)
		return tile;

	SDL_Point t_ids = map_system::world_to_tilemap_ids(world_coords, tc);

	tile = tc->terrain_tiles[t_ids.y][t_ids.x];

	return tile;
}

map_system::map_system()
{
}

map_system::~map_system()
{
}
