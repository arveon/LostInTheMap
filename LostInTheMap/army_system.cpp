#include "army_system.h"

std::vector<army_unit*> army_system::player_army;
std::vector<Entity*> army_system::unit_frames;
std::vector<Entity*> army_system::unit_frame_details;

SDL_Point army_system::panel{ 600, 15 };
int army_system::portrait_width = 48;

int army_system::a = 0;
//std::vector<army_unit*> army_system::battle_results_to_player;

void army_system::set_player_army(std::vector<army_unit*> pl)
{
	if (player_army.size() > 0)
	{
		for (army_unit* u : player_army)
			delete u;
	}
	player_army = pl;
}

void army_system::init_player_army_frames(Space* game_space)
{
	for (army_unit* u : player_army)
	{
		Entity* e = create_unit_frame(u);
		IDrawable* dc = (IDrawable*)e->get_component(Component::ComponentType::Drawable);

		render_system::add_object_to_queue(dc);
		game_space->objects.push_back(e);

	}
}

void army_system::add_to_player_army(army_unit* u, Space* game_space)
{
	player_army.push_back(u);
	Entity* e = create_unit_frame(u);
	IDrawable* dc = (IDrawable*)e->get_component(Component::ComponentType::Drawable);

	render_system::add_object_to_queue(dc);
	game_space->objects.push_back(e);
	a++;
}

Entity* army_system::create_unit_frame(army_unit* u)
{
	std::string name = "unit_" + std::to_string(u->type) + "_frame_" + std::to_string(a);
	Entity* frame = new Entity(entity_type::ui_element, name);
	frame->utility = u;

	Transform* tf = new Transform(frame);
	tf->position = { panel.x + a * portrait_width, panel.y, portrait_width, portrait_width };

	IDrawable* dc = new IDrawable(frame, IDrawable::layers::ui);
	std::string path = "assets/graphics/characters/portraits/" + NameToTypeConversion::get_character_name_by_type(u->type) + "_portrait.png";
	dc->sprite = asset_controller::load_texture(path.c_str());
	dc->draw_rect = tf->position;

	frame->transform = tf;
	frame->add_component(dc);

	army_system::update_frame_quantity(frame, u);
	army_system::create_unit_frame_details(frame, u);


	unit_frames.push_back(frame);
	a++;

	return frame;
}

Entity* army_system::create_unit_frame_details(Entity* frame, army_unit* u)
{
	std::string name = "frame_" + std::to_string(a) + "_details";
	Entity* frame_details = new Entity(entity_type::ui_element, name);

	//set description of frame to frame_details
	IDescriptable* fdc = new IDescriptable(frame);
	frame->add_component(fdc);
	fdc->description = frame_details;

	Transform* tf = new Transform(frame_details);
	tf->position = { panel.x + a * portrait_width, panel.y + portrait_width, (int)(portrait_width*1.5f), portrait_width * 2 };
	frame_details->add_component(tf);

	IDrawable* dc = new IDrawable(frame_details, IDrawable::layers::ui);
	dc->draw_rect = { tf->position.x, tf->position.y, tf->position.w, tf->position.h };
	frame_details->add_component(dc);
	dc->isActive = false;

#pragma region composing texture
	//compose unit stats frame texture
	SDL_Texture* bg = asset_controller::load_texture("assets/graphics/ui/icons/background.png");
	SDL_Texture* close_ico = asset_controller::load_texture("assets/graphics/ui/icons/close_damage_icon.png");
	SDL_Texture* health_ico = asset_controller::load_texture("assets/graphics/ui/icons/health_icon.png");
	SDL_Texture* speed_ico = asset_controller::load_texture("assets/graphics/ui/icons/speed_icon.png");

	//get all of the required text rendered
	std::string close_dmg_text = std::to_string(u->min_damage_close) + " - " + std::to_string(u->max_damage_close);
	SDL_Texture* close_dmg_tex = asset_controller::get_texture_from_text(close_dmg_text, UI_text_type::game_ui_small);
	SDL_Rect cdw = asset_controller::get_texture_size(close_dmg_tex);

	std::string health = std::to_string(u->max_health);
	SDL_Texture* health_tex = asset_controller::get_texture_from_text(health, UI_text_type::game_ui_small);
	SDL_Rect hw = asset_controller::get_texture_size(health_tex);

	std::string speed = std::to_string(u->speed);
	SDL_Texture* speed_tex = asset_controller::get_texture_from_text(speed, UI_text_type::game_ui_small);
	SDL_Rect sw = asset_controller::get_texture_size(speed_tex);

	std::vector<SDL_manager::Tile> textures;
	textures.push_back({ bg, { 0,0, tf->position.w, tf->position.h } });

	textures.push_back({ close_ico, { 10,30,10,10 } });
	textures.push_back({ close_dmg_tex, { 25,30, cdw.w, cdw.h } });

	textures.push_back({ health_ico,{ 10,45,10,10 } });
	textures.push_back({ health_tex,{ 25,45,hw.w, hw.h } });

	textures.push_back({ speed_ico,{ 10,60,10,10 } });
	textures.push_back({ speed_tex,{ 25,60,sw.w, sw.h } });

	dc->sprite = SDL_manager::create_terrain_texture(textures, tf->position.w, tf->position.h);

	//destroy all loaded textures as they are not required anymore
	for (SDL_manager::Tile t : textures)
		asset_controller::destroy_texture(t.tex);
#pragma endregion

	render_system::add_object_to_queue(dc);

	unit_frame_details.push_back(frame_details);
	return frame_details;
}

void army_system::update_frame_quantity(Entity* frame, army_unit* u)
{
	Transform* tf = frame->transform;

	IDrawable* frame_dc = (IDrawable*)frame->get_component(Component::ComponentType::Drawable);
	Transform* frame_tf = (Transform*)frame->get_component(Component::ComponentType::Transf);

	//get text and background
	std::string text = std::to_string(u->quantity);
	SDL_Texture* text_tex = asset_controller::get_texture_from_text(text, UI_text_type::game_ui_small, 32);
	SDL_Texture* text_bg = asset_controller::load_texture("default.png");

	SDL_Rect text_rect = asset_controller::get_texture_size(text_tex);
	text_rect.x = 15 / 2 - text_rect.w / 2;

	//text rendered onto the background
	SDL_Texture* combined_text_bg = asset_controller::get_texture_from_two(text_bg, text_tex, 15, text_rect.h, text_rect);

	//position of text on the final texture
	SDL_Rect quantity_position = {
		tf->position.w / 2 - text_rect.w / 2,
		tf->position.h - text_rect.h,
		15,
		text_rect.h
	};

	//store so it can be cleaned up later
	SDL_Texture* previous_sprite = frame_dc->sprite;

	frame_dc->sprite = asset_controller::get_texture_from_two(frame_dc->sprite, combined_text_bg, frame_tf->position.w, frame_tf->position.h, quantity_position);

	//clean up
	asset_controller::destroy_texture(text_tex);
	asset_controller::destroy_texture(text_bg);
	asset_controller::destroy_texture(combined_text_bg);
	asset_controller::destroy_texture(previous_sprite);
}

void army_system::update_mouse_over_frame(SDL_Point mouse_pos)
{
	for (Entity* frame : unit_frames)
	{
		if (frame->is_active)
		{
			Transform* tf = frame->transform;
			IDescriptable* idc = (IDescriptable*)frame->get_component(Component::ComponentType::Description);
			if (idc)
			{
				IDrawable* dc = (IDrawable*)idc->description->get_component(Component::ComponentType::Drawable);
				if (geometry_utilities::is_point_in_rect(mouse_pos.x, mouse_pos.y, tf->position))
					dc->isActive = true;
				else
					dc->isActive = false;
			}
		}
	}
}

void army_system::update_unit_frames()
{
	for (Entity* frame : unit_frames)
	{
		update_frame_quantity(frame, (army_unit*)frame->utility);
	}
}

army_system::army_system()
{
}


army_system::~army_system()
{
}
