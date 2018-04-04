#include "level_loading_system.h"

int level_loading_system::t_elapsed_time;
int level_loading_system::loading_progress;
level_loading_system::loading_state level_loading_system::loading_stage;
std::vector<void(*)()> level_loading_system::loading_done_listeners;
std::vector<xml_system::LoadingState> level_loading_system::loading_states;
levels level_loading_system::level_to_load;


void level_loading_system::load_new_game()
{

}

void level_loading_system::init_space(MenuLayout layout, Space & space, levels to_load)
{
	if (space.initialised)
		return;
	//temporary for testing
	t_elapsed_time = 0;
	
	//general system initialisation
	loading_stage = loading_state::loading_terrain;
	level_to_load = to_load;
	loading_states = xml_system::get_loading_states();
	loading_progress = 0;

#pragma region initialise_background
	Entity* background = new Entity(entity_type::background);

	Transform* bg_transform = new Transform(background);
	IDrawable* bg_draw = new IDrawable(background, IDrawable::layers::background);
	bg_draw->sprite = asset_controller::load_texture(layout.background_path.c_str());

	SDL_manager::get_window_size(&bg_transform->position.w, &bg_transform->position.h);
	bg_draw->draw_rect = { 0,0,bg_transform->position.w,bg_transform->position.h };
	//asset_controller::set_texture_alpha(draw->sprite, 0);

	background->add_component(bg_transform);
	background->add_component(bg_draw);
	space.objects.push_back(background);
	space.initialised = true;
#pragma endregion

#pragma region initialise all other elements
	for (unsigned int i = 0; i < layout.buttons.size(); i++)
	{
		ui_element_config elm = layout.buttons.at(i);
		Entity* element = new Entity(entity_type::ui_element);
		element->name = elm.name;

		Transform* element_transf = new Transform(element);
		element->transform = element_transf;
		element_transf->position.x = elm.position.x;
		element_transf->position.y = elm.position.y;

		IDrawable* element_dc = new IDrawable(element, IDrawable::layers::surface);
		element->add_component(element_dc);

		if (elm.type == UI_Element_Type::loading_bar)
		{
			element_dc->sprite = asset_controller::load_texture("assets/graphics/ui/load_bar.png");
			element_dc->draw_rect = asset_controller::get_texture_size(element_dc->sprite);
			element_dc->draw_rect = { elm.position.x, elm.position.y, element_dc->draw_rect.w*2, element_dc->draw_rect.h * 2 };
			element_transf->position = {elm.position.x, elm.position.y, element_dc->draw_rect.w, element_dc->draw_rect.h};

			Entity* fill = new Entity(entity_type::ui_element);
			fill->name = "Fill";
			//bar_fill
			Transform* fill_transf = new Transform(fill);
			fill->transform = fill_transf;

			IAnimatable* fill_anim = new IAnimatable(fill);
			fill_anim->spritesheet = asset_controller::load_texture("assets/graphics/ui/load_fill.png");
			//SDL_Rect temp = asset_controller::get_texture_size(fill_anim->spritesheet);
			fill_anim->src_rect = { 0,0,element_dc->draw_rect.w,element_dc->draw_rect.h };
			fill->add_component(fill_anim);

			IDrawable* fill_drawable = new IDrawable(fill, IDrawable::layers::foreground);
			fill_drawable->draw_rect = fill_anim->src_rect;
			fill_drawable->sprite = asset_controller::get_sprite_from_spritesheet(fill_anim->spritesheet, fill_anim->src_rect);
			fill_drawable->draw_rect.x = elm.position.x;
			fill_drawable->draw_rect.y = elm.position.y;
			fill->add_component(fill_drawable);

			fill_transf->position = { elm.position.x, elm.position.y, fill_drawable->draw_rect.w, fill_drawable->draw_rect.h };
			space.objects.push_back(fill);
		}
		else if (elm.type == UI_Element_Type::text)
		{
			if (element->name.compare("Status") == 0)
			{
				element_dc->sprite = asset_controller::get_texture_from_text("Loading map", UI_text_type::main_menu_text);
			}
			else if (element->name.compare("Progress") == 0)
			{
				element_dc->sprite = asset_controller::get_texture_from_text("0%", UI_text_type::main_menu_text);
			}

			SDL_Rect temp = asset_controller::get_texture_size(element_dc->sprite);
			element_dc->draw_rect = { element_transf->position.x, element_transf->position.y, temp.w, temp.h };


		}

		space.objects.push_back(element);
	}
#pragma endregion

	add_space_to_render_queue(space);
}

void level_loading_system::update_space(Space & space, Space & level_space, int dt)
{
	if (loading_stage == loading_state::done)
	{
		loading_done();
		return;
	}
	update_bar_fill(space);
	load_game_components(level_space);
}

void level_loading_system::loading_done()
{
	for (unsigned int i = 0; i < loading_done_listeners.size(); i++)
	{
		void(*listener)() = loading_done_listeners.at(i);
		listener();
	}
}

void level_loading_system::update_bar_fill(Space& space)
{
	Entity* fill = SpaceSystem::find_entity_by_name(space, "Fill");
	if (!fill)
		return;

	IDrawable* draw = static_cast<IDrawable*>(fill->get_component(Component::ComponentType::Drawable));
	IAnimatable* anim = static_cast<IAnimatable*>(fill->get_component(Component::ComponentType::Animated));
	Transform* tr = static_cast<Transform*>(fill->get_component(Component::ComponentType::Transf));
	//get whole spritesheet width
	SDL_Rect ss_size = asset_controller::get_texture_size(anim->spritesheet);

	//calculate desired width
	int d_w = tr->position.w * loading_progress / 100;

	//make src_rect.w equal to that width
	anim->src_rect.w = d_w / 2;

	//make draw_rect.w equal to that width
	draw->draw_rect.w = d_w;

	//remove old sprite
	asset_controller::destroy_texture(draw->sprite);

	//update sprite to new sprite
	draw->sprite = asset_controller::get_sprite_from_spritesheet(anim->spritesheet, anim->src_rect);
}

void level_loading_system::update_status_text(Space & space, int stage_id)
{
	Entity* status = SpaceSystem::find_entity_by_name(space, "Status");
	if (!status)
		return;

	IDrawable* dc = static_cast<IDrawable*>(status->get_component(Component::ComponentType::Drawable));
	if (!dc)
		return;

	Transform* tc = static_cast<Transform*>(status->get_component(Component::ComponentType::Transf));
	if (!dc)
		return;

	//this bit ensures the text is always in the x middle of the screen
	SDL_manager::get_window_size(&tc->position.x, nullptr);
	tc->position.x /= 2;

	asset_controller::destroy_texture(dc->sprite);
	dc->sprite = asset_controller::get_texture_from_text(loading_states.at(stage_id-1).name, UI_text_type::main_menu_text);
	SDL_Rect temp = asset_controller::get_texture_size(dc->sprite);

	tc->position.w = temp.w;
	tc->position.h = temp.h;
	tc->position.x -= temp.w / 2;

	dc->draw_rect = tc->position;
}

void level_loading_system::load_game_components(Space & game_space)
{
	switch (loading_stage)
	{
	case loading_state::loading_terrain:
	{
		

		int w, h, tw;
		int** map_tile_ids = xml_system::load_map_tiles(level_to_load, &w, &h, &tw);
		Entity * terrain = new Entity(entity_type::tilemap, "terrain");
		ITerrain* tc = new ITerrain(terrain);
		tc->width = w;
		tc->height = h;
		tc->tile_width = tw;
		terrain->add_component(tc);
		map_system::init_terrain_map(map_tile_ids, level_to_load, terrain);
		game_space.objects.push_back(terrain);

		camera_system::init_camera(tw);
	}
		break;
	case loading_state::creating_terrain_collisions:
	{
		Entity* tr = SpaceSystem::find_entity_by_name(game_space, "terrain");
		ITerrain* terrain = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));
		int** map_tile_collisions = xml_system::load_map_collisions(level_to_load, terrain->width, terrain->height);
		map_system::init_terrain_collisions(map_tile_collisions, tr);
	}
		break;
	case loading_state::initialising_pathfinding:
	{
		Entity* tilemap = SpaceSystem::find_entity_by_name(game_space, "terrain");
		ITerrain* terrain = static_cast<ITerrain*>(tilemap->get_component(Component::ComponentType::Terrain));

		lee_pathfinder::init_pathfinder(map_system::get_pathfinding_map(terrain), terrain->width, terrain->height);
	}
	break;
	case loading_state::loading_objects:
	{
		
	}
		break;
	case loading_state::loading_characters:
	{
		Entity* tr = SpaceSystem::find_entity_by_name(game_space, "terrain");
		ITerrain* terrain = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));
		Character** characters = xml_system::load_characters(level_to_load, terrain->width, terrain->height);
		std::vector<Entity*> temp = character_system::init_characters(characters, terrain->width, terrain->height, terrain);
		for (unsigned int i = 0; i < temp.size(); i++)
			game_space.objects.push_back(temp.at(i));

	}
		break;
	case loading_state::loading_terrain_textures:
	{
		std::string level_name;
		switch (level_to_load)
		{
		case levels::test:
			level_name = "test";
			break;
		case levels::pyramid:
			level_name = "pyramid";
			break;
		}
		Entity* tr = SpaceSystem::find_entity_by_name(game_space, "terrain");
		ITerrain* terrain = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));

		asset_controller::load_terrain_textures("assets/tilesets/" + level_name + ".png", terrain->tile_width);
	}
		break;
	case loading_state::loading_item_textures:
	{

	}
		break;
	case loading_state::attaching_terrain_textures:
	{
		Entity* tr = SpaceSystem::find_entity_by_name(game_space, "terrain");
		ITerrain* terrain = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));

		for (int i = 0; i < terrain->height; i++)
			for (int j = 0; j < terrain->width; j++)
			{
				if (!terrain->terrain_tiles[i][j])
					continue;
				Transform* tf = static_cast<Transform*>(terrain->terrain_tiles[i][j]->get_component(Component::ComponentType::Transf));
				IDrawable* dc = static_cast<IDrawable*>(terrain->terrain_tiles[i][j]->get_component(Component::ComponentType::Drawable));
				dc->sprite = asset_controller::get_terrain_texture(dc->id);
				dc->draw_rect = camera_system::world_to_camera_space(tf->position, dc->draw_rect);
				dc->draw_rect.w = dc->draw_rect.h = terrain->tile_width;
				dc->sprite_origin = { terrain->tile_width / 2, terrain->tile_width/2 };
			}
	}
		break;
	case loading_state::attaching_character_textures:
	{
		Entity* tr = SpaceSystem::find_entity_by_name(game_space, "terrain");
		ITerrain* terrain = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));

		SDL_Point origin;
		for (int i = 0; i < terrain->height; i++)
			for (int j = 0; j < terrain->width; j++)
			{
				if (!terrain->terrain_tiles[i][j])
					continue;
				else
				{
					IDrawable* tt = static_cast<IDrawable*>(terrain->terrain_tiles[i][j]->get_component(Component::ComponentType::Drawable));
					if (!tt)
						continue;
					origin = tt->sprite_origin;
				}
			}
		character_system::attach_textures_to_characters(origin);
	}
		break;
	case loading_state::attaching_object_textures:
	{

	}
		break;
	case loading_state::cleaning_up_tilesheet:
	{

	}
		break;
	case loading_state::loading_misc:
	{

	}
	case loading_state::objects_camera_positions:
	{
		Entity* tr = SpaceSystem::find_entity_by_name(game_space, "terrain");
		ITerrain* terrain = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));

		for (int i = 0; i < terrain->height; i++)
			for (int j = 0; j < terrain->width; j++)
			{
				if (!terrain->terrain_tiles[i][j])
					continue;
				Transform* tf = static_cast<Transform*>(terrain->terrain_tiles[i][j]->get_component(Component::ComponentType::Transf));
				IDrawable* dc = static_cast<IDrawable*>(terrain->terrain_tiles[i][j]->get_component(Component::ComponentType::Drawable));
				dc->draw_rect = camera_system::world_to_camera_space(tf->position, dc->draw_rect);
			}

		for (unsigned int i = 0; i < game_space.objects.size(); i++)
		{
			Entity* e = game_space.objects.at(i);

			Transform* tr = static_cast<Transform*>(e->get_component(Component::ComponentType::Transf));
			if (!tr)
				continue;
			IDrawable* dc = static_cast<IDrawable*>(e->get_component(Component::ComponentType::Drawable));
			if (!dc)
				continue;

			dc->draw_rect = camera_system::world_to_camera_space(tr->position, dc->draw_rect);
		}

		camera_system::set_camera_target(SpaceSystem::find_entity_by_name(game_space, "player"));
	}
		break;
	case loading_state::done:
	{
		for (unsigned int i = 0; i < loading_done_listeners.size(); i++)
		{
			void(*a)() = loading_done_listeners.at(i);
			a();
		}
	}
		break;
	}

	int stage_id = static_cast<int>(loading_stage);
	int max_stage = static_cast<int>(loading_state::done);
	if (stage_id + 1 < max_stage)
	{
		loading_stage = static_cast<loading_state>(++stage_id);
		loading_progress = (float)loading_states.at(stage_id).value;
	}
	else
	{
		loading_progress = (float)loading_states.at(max_stage-1).value;
		loading_stage = loading_state::done;
		stage_id = max_stage;
	}
	update_status_text(game_space, stage_id);

}


level_loading_system::level_loading_system()
{
}


level_loading_system::~level_loading_system()
{
}
