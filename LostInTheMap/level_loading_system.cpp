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
	director::cur_level = to_load;
	
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
	/*if (loading_stage == loading_state::done)
	{
		loading_done();
		return;
	}*/
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
	static std::vector<Entity*> objects;
	switch (loading_stage)
	{
	case loading_state::loading_terrain:
	{
		int w, h, tw, th;
		int** map_tile_ids = xml_system::load_map_tiles(level_to_load, &w, &h, &tw, &th);
		Entity * terrain = new Entity(entity_type::tilemap, "terrain");
		ITerrain* tc = new ITerrain(terrain);
		tc->width = w;
		tc->height = h;
		tc->tile_width = tw;
		tc->tile_height = th;
		terrain->add_component(tc);
		map_system::init_terrain_map(map_tile_ids, terrain);
		game_space.objects.push_back(terrain);

		camera_system::init_camera(tw,tc->width*tc->tile_width, tc->height*tc->tile_height);
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
		Actor** actors = xml_system::load_characters_and_objects(level_to_load, terrain->width, terrain->height);
		std::vector<Entity*> temp = character_system::init_characters(actors, terrain->width, terrain->height, terrain, true);
		for (Entity* a : temp)
			game_space.objects.push_back(a);

		//load triggers
		std::vector<Entity*> temp_t = map_system::init_triggers(actors, terrain);
		for (Entity* a : temp_t)
			game_space.objects.push_back(a);

		std::vector<Entity*> temp_obj = map_system::init_objects(actors, terrain);
		for (Entity* a : temp_obj)
		{
			objects.push_back(a);
			game_space.objects.push_back(a);
		}

	}
		break;
	case loading_state::loading_terrain_textures:
	{
		std::string level_name = NameToTypeConversion::get_level_name_str(level_to_load);
		Entity* tr = SpaceSystem::find_entity_by_name(game_space, "terrain");
		ITerrain* terrain = static_cast<ITerrain*>(tr->get_component(Component::ComponentType::Terrain));

		asset_controller::load_terrain_textures("assets/tilesets/" + level_name + ".png", terrain_texture_width, terrain_texture_height);
	}
		break;
	case loading_state::loading_object_textures:
	{
		for (Entity* a : objects)
		{
			IInteractionSource* oo = static_cast<IInteractionSource*>(a->get_component(Component::ComponentType::InteractionSource));
			SDL_Texture* tex = asset_controller::get_object_texture(NameToTypeConversion::get_object_name_by_type(oo->o_type));
			IDrawable* dc = static_cast<IDrawable*>(a->get_component(Component::ComponentType::Drawable));
			IAnimatable* ac = static_cast<IAnimatable*>(a->get_component(Component::ComponentType::Animated));
			ac->spritesheet = tex;
			dc->sprite = asset_controller::get_sprite_from_spritesheet(ac->spritesheet, ac->src_rect);
		}
		
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
				dc->draw_rect.w = terrain->tile_width;
				dc->draw_rect.h = terrain->tile_height;
				dc->sprite_origin = { terrain->tile_width / 2, terrain->tile_height /2 };
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
		//create inactive dialog panel
		Entity* dialog_panel = new Entity(entity_type::ui_element, "dialogue_panel");
		
		IDrawable* dc_panel = new IDrawable(dialog_panel, IDrawable::layers::ui);
		xml_system::DialogueFrame frame = xml_system::load_dialogue_frame();
		dc_panel->draw_rect = frame.bg_frame;
		dc_panel->sprite = asset_controller::load_texture(frame.bg_path.c_str());
		dc_panel->isActive = false;
		dialog_panel->add_component(dc_panel);

		Entity* dialog_portrait = new Entity(entity_type::ui_element, "dialogue_portrait");
		IDrawable* dc_portrait = new IDrawable(dialog_portrait, IDrawable::layers::ui);
		dc_portrait->draw_rect = frame.portrait_frame;
		dc_portrait->isActive = false;
		dialog_portrait->add_component(dc_portrait);

		Entity* dialog_text = new Entity(entity_type::ui_element, "dialogue_text");
		IDrawable* dc_text = new IDrawable(dialog_portrait, IDrawable::layers::ui);
		dc_text->draw_rect = frame.text_frame;
		dc_text->isActive = false;
		dialog_text->add_component(dc_text);

		game_space.objects.push_back(dialog_panel);
		game_space.objects.push_back(dialog_portrait);
		game_space.objects.push_back(dialog_text);

		dialogue_system::max_text_width = dc_text->draw_rect.w;

		script_system::game_space = &game_space;
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
		objects.clear();
		director::init_stage(level_to_load);
		script_system::set_combat_start_callback(&level_loading_system::load_combat);
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

//is called when combat start script action is invoked
void level_loading_system::load_combat(levels level, Space& game_space, IFightable* fc)
{
	Entity* player = SpaceSystem::find_entity_by_name(game_space, "player");

	//load terrain
	int w, h, tw, th;
	int** map = nullptr;
	int** collisions;
	if (level == levels::pyramid)
	{
		map = xml_system::load_map_tiles(levels::pyramid, &w, &h, &tw, &th, true);
		collisions = xml_system::load_map_collisions(levels::pyramid, w, h, true);
	}

	Entity* tilemap = new Entity(entity_type::game_object_combat, "cb_terrain");
	ITerrain* tc = new ITerrain(tilemap);
	tc->width = w;
	tc->height = h;
	tc->tile_width = tw;
	tc->tile_height = th;
	tilemap->add_component(tc);
	//create entities for all tiles
	map_system::init_terrain_map(map, tilemap);

	//create combat terrain
	for (int i = 0; i < tc->height; i++)
		for (int j = 0; j < tc->width; j++)
		{
			if (!tc->terrain_tiles[i][j])
				continue;
			Transform* tf = static_cast<Transform*>(tc->terrain_tiles[i][j]->get_component(Component::ComponentType::Transf));
			tf->position = 
			{
				j * tc->tile_width,
				i * tc->tile_height,
				tc->tile_width,
				tc->tile_height
			};
			IDrawable* dc = static_cast<IDrawable*>(tc->terrain_tiles[i][j]->get_component(Component::ComponentType::Drawable));
			dc->sprite = asset_controller::get_terrain_texture(dc->id);
			dc->draw_rect = tf->position;
			dc->draw_rect.w = tc->tile_width;
			dc->draw_rect.h = tc->tile_height;
			dc->sprite_origin = { tc->tile_width / 2, tc->tile_height };
		}
	game_space.objects.push_back(tilemap);

	//create combat units
	//for enemy
	load_character_army(tc, game_space, true, fc);
	//for player
	load_character_army(tc, game_space, false);

	combat_flow::set_in_combat();
}

void level_loading_system::load_character_army(ITerrain* tc, Space& game_space, bool enemy, IFightable* character_fc)
{
	std::vector<army_unit*> army;
	//for player
	if (enemy)
	{
		army = xml_system::load_army(character_fc->army_file, level_to_load);
		combat_flow::init_enemy_army(army);
	}
	else
		army = combat_flow::player_army;

	//std::vector<army_unit> army = combat_flow::player_army;

	//how many units
	int size = army.size();
	int total_tiles = tc->width;
	//what the tile distances between them
	int distances = std::floor(total_tiles / (float)army.size());
	int id = 0;
	for (int i = 0; i < army.size(); i++)
	{
		army_unit* u = army.at(i);
		if (army.at(i)->health_of_first == 0)
			continue;
		u->is_enemy = enemy;
		Entity* unit = character_system::load_combat_character(distances, id, tc, u, enemy);
		if (enemy)
			combat_flow::enemy_army.at(i)->unit_entity = unit;
		else
			combat_flow::player_army.at(i)->unit_entity = unit;
		game_space.objects.push_back(unit);
		id++;
	}
}

level_loading_system::level_loading_system()
{
}


level_loading_system::~level_loading_system()
{
}
