#include "army_system.h"

std::vector<army_unit*> army_system::player_army;
std::vector<Entity*> army_system::unit_frames;

SDL_Point army_system::panel{600, 15};
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

	Transform* tf = new Transform(frame);
	tf->position = { panel.x + a * portrait_width, panel.y, portrait_width, portrait_width };

	IDrawable* dc = new IDrawable(frame, IDrawable::layers::ui);
	std::string path = "assets/graphics/characters/portraits/" + NameToTypeConversion::get_character_name_by_type(u->type) + "_portrait.png";
	dc->sprite = asset_controller::load_texture(path.c_str());
	dc->draw_rect = tf->position;

	//description
	IDescriptable* desc = new IDescriptable(frame);
	/*desc->text = std::to_string(u->quantity);
	desc->rendered_text = asset_controller::get_texture_from_text(desc->text, UI_text_type::game_ui_small, 32);*/
	desc->box_background = asset_controller::load_texture("aaa");
	desc->described_object = u;
	
	//description entity
	Entity* unit_description = new Entity(entity_type::ui_element, frame->name + "_quantity");
	IDrawable* desc_dc = new IDrawable(unit_description, IDrawable::layers::ui);
	Transform* desc_tc = new Transform(unit_description);

	unit_description->add_component(desc_dc);
	unit_description->add_component(desc_tc);
	desc->description = unit_description;
	frame->transform = tf;
	frame->add_component(dc);
	frame->add_component(desc);
	
	army_system::update_frame_quantity(frame);
	render_system::add_object_to_queue(desc_dc);

	unit_frames.push_back(frame);
	a++;

	return frame;
}

void army_system::update_frame_quantity(Entity* frame)
{
	Transform* tf = frame->transform;

	IDescriptable* desc = (IDescriptable*)frame->get_component(Component::ComponentType::Description);
	if (!desc)
		return;

	army_unit* u = (army_unit*)desc->described_object;
	desc->text = std::to_string(u->quantity);
	desc->rendered_text = asset_controller::get_texture_from_text(desc->text, UI_text_type::game_ui_small, 32);

	Transform* desc_tc = (Transform*)desc->description->get_component(Component::ComponentType::Transf);
	IDrawable* desc_dc = (IDrawable*)desc->description->get_component(Component::ComponentType::Drawable);

	SDL_Rect text_rect = asset_controller::get_texture_size(desc->rendered_text);
	desc_tc->position = {
		tf->position.x + tf->position.w - tf->position.w / 2,
		tf->position.y + tf->position.h - text_rect.h,
		15,
		text_rect.h
	};

	desc_dc->draw_rect = desc_tc->position;
	desc_dc->sprite = asset_controller::get_texture_from_two(desc->box_background, desc->rendered_text, desc_tc->position.w, desc_tc->position.h, text_rect);	
}

void army_system::update_unit_frames()
{
	for (Entity* frame : unit_frames)
	{
		update_frame_quantity(frame);
	}
}

army_system::army_system()
{
}


army_system::~army_system()
{
}
