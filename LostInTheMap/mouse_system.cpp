#include "mouse_system.h"

mouse_system::mouse_icons mouse_system::cur_icon;

Entity* mouse_system::create_mouse()
{
	Entity* mouse = new Entity(entity_type::mouse, "mouse");
	Transform* m_transform = new Transform(mouse);
	m_transform->position = { 0,0,1,1 };
	m_transform->origin = { 0,0 };

	IAnimatable* m_anim_comp = new IAnimatable(mouse);
	m_anim_comp->spritesheet = asset_controller::load_texture("assets/graphics/ui/mouse.png");
	m_anim_comp->src_rect = { 0,0,16,16 };
	m_anim_comp->sprite_changed = false;

	IDrawable* m_draw_comp = new IDrawable(mouse, IDrawable::layers::mouse);
	m_draw_comp->sprite = asset_controller::get_sprite_from_spritesheet(m_anim_comp->spritesheet, m_anim_comp->src_rect);

	m_draw_comp->draw_rect = asset_controller::get_texture_size(m_draw_comp->sprite);

	IMouse* mc = new IMouse(mouse);

	mouse->transform = m_transform;
	mouse->add_component(m_draw_comp);
	mouse->add_component(mc);
	mouse->add_component(m_anim_comp);

	cur_icon = mouse_icons::normal;

	return mouse;
}

void mouse_system::update_mouse(Entity* mouse, Space& space, bool in_dialogue, bool hidden)
{
	IDrawable* dc = static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable));
	IAnimatable* ac = static_cast<IAnimatable*>(mouse->get_component(Component::ComponentType::Animated));
	IMouse* mc = static_cast<IMouse*>(mouse->get_component(Component::ComponentType::Mouse));
	Transform* mt = mouse->transform;

	mouse->transform->position.x = input_system::mouse.x;
	mouse->transform->position.y = input_system::mouse.y;
	dc->draw_rect.x = input_system::mouse.x - mt->origin.x;
	dc->draw_rect.y = input_system::mouse.y - mt->origin.y;

	if (!in_dialogue)
	{
		ITerrain* terrain = SpaceSystem::get_terrain(space);
		int x_cap = terrain->width * terrain->tile_width - 1;
		int y_cap = terrain->height * terrain->tile_height - 1;

		SDL_Point mouse_world = mouse_system::get_mouse_in_world(mouse);
		int dx, dy;
		dx = x_cap - mouse_world.x;
		dy = y_cap - mouse_world.y;
		if (dx < 0)
		{
			dc->draw_rect.x -= dx;
			mt->position.x -= dx;
		}
		if (dy < 0)
		{
			dc->draw_rect.y -= dy;
			mt->position.y -= dy;
		}

		Entity* target_object = SpaceSystem::get_object_at_point(space, mouse_world.x, mouse_world.y, true);
		Entity* tile = SpaceSystem::get_tile_at_point(space, mouse_world.x, mouse_world.y);
		if (tile)
		{
			ITile* tlc = static_cast<ITile*>(tile->get_component(Component::ComponentType::Tile));
			if (target_object && tlc->is_traversible)
			{
				ITile* tc = static_cast<ITile*>(target_object->get_component(Component::ComponentType::Tile));
				ICharacter* cc = static_cast<ICharacter*>(target_object->get_component(Component::ComponentType::Character));
				IInteractionSource* interaction = static_cast<IInteractionSource*>(target_object->get_component(Component::ComponentType::InteractionSource));
				if (!target_object->is_active)
				{
					mc->cur_target = target_object;
					change_mouse_icon(mouse_icons::walking, ac, dc);
				}
				else if (tc)
				{
					if (tc->is_traversible)
					{
						mc->cur_target = target_object;
						change_mouse_icon(mouse_icons::walking, ac, dc);
					}
					else
					{
						mc->cur_target = nullptr;
						change_mouse_icon(mouse_icons::blocked, ac, dc);
					}
				}
				else if (cc)
				{
					mc->cur_target = target_object;
					if (cc->is_friendly)
						change_mouse_icon(mouse_icons::talk, ac, dc);
					else
						change_mouse_icon(mouse_icons::attack, ac, dc);
				}
				else if (interaction)
				{
					mc->cur_target = target_object;
					change_mouse_icon(mouse_icons::normal, ac, dc);
				}
			}
			else
			{
				mc->cur_target = nullptr;
				change_mouse_icon(mouse_icons::blocked, ac, dc);
			}
		}
		else
		{
			mc->cur_target = nullptr;
			change_mouse_icon(mouse_icons::blocked, ac, dc);
		}
	}
	else
	{
		change_mouse_icon(mouse_icons::normal, ac, dc);
	}

	if (hidden && !in_dialogue)
		dc->isActive = false;
	else
		dc->isActive = true;
}

void mouse_system::update_mouse_combat(Entity * mouse, Space & space, int steps_allowed, Entity* cur_unit)
{
	IDrawable* dc = static_cast<IDrawable*>(mouse->get_component(Component::ComponentType::Drawable));
	IAnimatable* ac = static_cast<IAnimatable*>(mouse->get_component(Component::ComponentType::Animated));
	IMouse* mc = static_cast<IMouse*>(mouse->get_component(Component::ComponentType::Mouse));
	Transform* mt = mouse->transform;

	mouse->transform->position.x = input_system::mouse.x;
	mouse->transform->position.y = input_system::mouse.y;
	dc->draw_rect.x = input_system::mouse.x - mt->origin.x;
	dc->draw_rect.y = input_system::mouse.y - mt->origin.y;

	IMoving* movement_component = static_cast<IMoving*>(cur_unit->get_component(Component::ComponentType::Movement));
	if(movement_component->destination_reached)
	{
		ITerrain* tc = SpaceSystem::get_terrain(space);

		movement_component->pathfinder.set_origin(map_system::world_to_tilemap_ids(cur_unit->get_origin_in_world(),tc));

		std::vector<SDL_Point> path = movement_component->pathfinder.get_path_to(mouse_system::get_mouse_ids(mouse, tc), true);
		if (path.size() <= steps_allowed)
		{
			movement_component->path = path;
			change_mouse_icon(mouse_system::mouse_icons::walking, ac, dc);
		}
		else
		{
			movement_component->path.clear();
			change_mouse_icon(mouse_system::mouse_icons::normal, ac, dc);
		}
	}
}

SDL_Point mouse_system::get_mouse_ids(Entity* mouse, ITerrain* tc)
{
	SDL_Point result;
	SDL_Point world = mouse_system::get_mouse_in_world(mouse);

	result = map_system::world_to_tilemap_ids(world, tc);
	return result;
}

SDL_Point mouse_system::get_mouse_in_world(Entity * mouse)
{
	IMouse* ms = static_cast<IMouse*>(mouse->get_component(Component::ComponentType::Mouse));
	//get mouse screen space
	SDL_Point mouse_pos = { mouse->transform->position.x, mouse->transform->position.y };
	//get mouse world space
	mouse_pos = camera_system::screen_to_world_space(mouse_pos);

	return mouse_pos;
}

void mouse_system::change_mouse_icon(mouse_icons icon, IAnimatable* anim_component, IDrawable* draw_component)
{
	if (cur_icon == icon)
		return;
	Transform* tf = static_cast<Transform*>(anim_component->owner->get_component(Component::ComponentType::Transf));
	if (!tf)
		return;
	switch (icon)
	{
	case mouse_icons::normal:
		cur_icon = mouse_icons::normal;
		tf->origin = { 0,0 };
		anim_component->src_rect.x = 0;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 16;
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::walking:
		cur_icon = mouse_icons::walking;
		anim_component->src_rect.x = 16;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 24;
		tf->origin = { draw_component->draw_rect.w / 2, draw_component->draw_rect.h / 2 };
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::attack:
		cur_icon = mouse_icons::attack;
		anim_component->src_rect.x = 32;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 32;
		tf->origin = { draw_component->draw_rect.w / 2, draw_component->draw_rect.h / 2 };
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::blocked:
		cur_icon = mouse_icons::blocked;
		anim_component->src_rect.x = 48;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 24;
		tf->origin = { draw_component->draw_rect.w / 2, draw_component->draw_rect.h / 2 };
		anim_component->sprite_changed = true;
		break;
	case mouse_icons::talk:
		cur_icon = mouse_icons::talk;
		anim_component->src_rect.x = 64;
		draw_component->draw_rect.w = draw_component->draw_rect.h = 32;
		tf->origin = { draw_component->draw_rect.w / 2, draw_component->draw_rect.h / 2 };
		anim_component->sprite_changed = true;
		break;
	}


}


mouse_system::mouse_system()
{
}


mouse_system::~mouse_system()
{
}
