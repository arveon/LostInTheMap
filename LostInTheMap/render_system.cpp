#include "render_system.h"

std::vector<IDrawable*> render_system::background;
std::vector<IDrawable*> render_system::terrain;
std::vector<IDrawable*> render_system::foreground;
std::vector<IDrawable*> render_system::surface;
std::vector<IDrawable*> render_system::world_ui;
std::vector<IDrawable*> render_system::ui;
IDrawable* render_system::mouse;
SDL_Texture* render_system::terrain_sprite;
bool render_system::terrain_prepared;

int render_system::add_object_to_queue(IDrawable * obj)
{
	if (obj->owner->name.compare("mouse") == 0 || obj->owner->name.compare("cb_mouse") == 0)
	{
		render_system::mouse = obj;
		return 0;
	}
	int id = -1;
	switch (obj->layer)
	{
	case IDrawable::layers::background:
		background.push_back(obj);
		id = background.size()-1;
		break;
	case IDrawable::layers::terrain:
		terrain.push_back(obj);
		id = terrain.size() - 1;
		break;
	case IDrawable::layers::surface:
		surface.push_back(obj);
		id = surface.size()-1;
		break;
	case IDrawable::layers::foreground:
		foreground.push_back(obj);
		id = foreground.size()-1;
		break;
	case IDrawable::layers::world_ui:
		world_ui.push_back(obj);
		id = world_ui.size() - 1;
		break;
	case IDrawable::layers::ui:
		ui.push_back(obj);
		id = ui.size() - 1;
		break;
	}
	return id;
}

void render_system::prepare_terrain(int map_width, int map_height)
{
	std::vector<SDL_manager::Tile> tiles;
	for (IDrawable* tile : terrain)
	{
		tiles.push_back({tile->sprite, tile->draw_rect});
	}
	terrain_sprite = SDL_manager::create_terrain_texture(tiles, map_width, map_height);
	terrain_prepared = true;
}

void render_system::unprepare_terrain()
{
	asset_controller::destroy_texture(terrain_sprite);
	terrain_prepared = false;
}

void render_system::flush_queues()
{
	background.clear();
	terrain.clear();
	surface.clear();
	foreground.clear();
	ui.clear();
	asset_controller::destroy_texture(terrain_sprite);
	terrain_prepared = false;
	mouse = nullptr;
}

void render_system::sort_queues()
{
	//only need to sort the surface queue
	bool swapped = true;
	if (surface.size() <= 0)
		goto ui;
	while (swapped)
	{
		swapped = false;

		for (unsigned int i = 0; i < surface.size() - 1; i++)
		{
			IDrawable* dr1 = surface.at(i);
			IDrawable* dr2 = surface.at(i + 1);

			int first_y = dr1->draw_rect.y + dr1->sprite_origin.y;
			int second_y = dr2->draw_rect.y + dr2->sprite_origin.y;

			if (first_y > second_y)
			{
				//swap ID's so that the draw components always contain their up-to-date version
				dr1->id++;
				dr2->id--;

				surface.at(i) = dr2;
				surface.at(i+1) = dr1;
				swapped = true;
			}
		}
	}

	ui:
	//sort ui queue as well
	if (ui.size() <= 0)
		return;
	swapped = true;
	while (swapped)
	{
		swapped = false;

		for (unsigned int i = 0; i < ui.size() - 1; i++)
		{
			IDrawable* dr1 = ui.at(i);
			IDrawable* dr2 = ui.at(i + 1);

			int first_y = dr1->draw_rect.y + dr1->sprite_origin.y;
			int second_y = dr2->draw_rect.y + dr2->sprite_origin.y;

			if (first_y > second_y)
			{
				ui.at(i) = dr2;
				ui.at(i + 1) = dr1;
				swapped = true;
			}
		}
	}
}

void render_system::render_queues()
{
	float zoom = camera_system::get_camera_zoom();
	SDL_Rect camera = camera_system::get_camera_rect();

	SDL_manager::start_render();
	if (background.size() > 0)
	{
		for (std::vector<IDrawable*>::iterator it = background.begin(); it != background.end(); it++)
		{
			IDrawable* obj = *it;
			if (!obj->isActive)
				continue;
			SDL_Rect dr = obj->draw_rect;
			dr.x = static_cast<int>(std::floor((float)(dr.x - camera.x) * zoom));
			dr.y = static_cast<int>(std::floor((float)(dr.y - camera.y) * zoom));
			dr.w = static_cast<int>(std::floor(dr.w * zoom)) + 1;//+1 is required to avoid tearing when zooming in/out
			dr.h = static_cast<int>(std::floor(dr.h * zoom)) + 1;//+1 is required to avoid tearing when zooming in/out
			if (dr.x + dr.w < 0 || dr.y + dr.h < 0)
				continue;
			
			SDL_manager::render_sprite(obj->sprite, dr);
		}
	}

	if (terrain.size() > 0)
	{
		if(terrain_prepared)
			SDL_manager::render_sprite_src(terrain_sprite, camera);
		else
		{
			for (std::vector<IDrawable*>::iterator it = terrain.begin(); it != terrain.end(); it++)
			{
				IDrawable* obj = *it;
				if (!obj->isActive)
					continue;
				SDL_Rect dr = obj->draw_rect;
				dr.x = static_cast<int>(std::floor((float)(dr.x - camera.x) * zoom));
				dr.y = static_cast<int>(std::floor((float)(dr.y - camera.y) * zoom));
				dr.w = static_cast<int>(std::floor(dr.w * zoom)) + 1;//+1 is required to avoid tearing when zooming in/out
				dr.h = static_cast<int>(std::floor(dr.h * zoom)) + 1;//+1 is required to avoid tearing when zooming in/out
				/*if (dr.x + dr.w < 0 || dr.x > camera.x + camera.w || dr.y + dr.h < 0 || dr.y > camera.y + camera.h)
					continue;*/
				
				SDL_manager::render_sprite(obj->sprite, dr);
			}
		}
	}

	if (surface.size() > 0)
	{
		for (std::vector<IDrawable*>::iterator it = surface.begin(); it != surface.end(); it++)
		{
			IDrawable* obj = *it;
			if (!obj->isActive)
				continue;
			SDL_Rect dr = obj->draw_rect;
			dr.x = static_cast<int>(std::floor((float)(dr.x - camera.x) * zoom));
			dr.y = static_cast<int>(std::floor((float)(dr.y - camera.y) * zoom));
			dr.w = static_cast<int>(std::floor(dr.w * zoom)) + 1;//+1 is required to avoid tearing when zooming in/out
			dr.h = static_cast<int>(std::floor(dr.h * zoom)) + 1;//+1 is required to avoid tearing when zooming in/out
			if (dr.x + dr.w < 0 || dr.y + dr.h < 0)
				continue;

			SDL_manager::render_sprite(obj->sprite, dr, obj->flipped_x);
		}
	}

	if (foreground.size() > 0)
	{
		for (std::vector<IDrawable*>::iterator it = foreground.begin(); it != foreground.end(); it++)
		{
			IDrawable* obj = *it;
			if (!obj->isActive)
				continue;
			SDL_Rect dr = obj->draw_rect;
			
			if (dr.x + dr.w < 0 || dr.y + dr.h < 0)
				continue;

			SDL_manager::render_sprite(obj->sprite, dr);
		}
	}

	for (IDrawable* obj : world_ui)
	{
		if (!obj->isActive)
			continue;
		SDL_Rect dr = obj->draw_rect;
		dr.x = static_cast<int>(std::floor((float)(dr.x - camera.x) * zoom));
		dr.y = static_cast<int>(std::floor((float)(dr.y - camera.y) * zoom));
		dr.w = static_cast<int>(std::floor(dr.w * zoom)) + 1;//+1 is required to avoid tearing when zooming in/out
		dr.h = static_cast<int>(std::floor(dr.h * zoom)) + 1;//+1 is required to avoid tearing when zooming in/out
		if (dr.x + dr.w < 0 || dr.y + dr.h < 0)
			continue;

		SDL_manager::render_sprite(obj->sprite, dr);
	}

	if (ui.size() > 0)
	{
		for (std::vector<IDrawable*>::iterator it = ui.begin(); it != ui.end(); it++)
		{
			IDrawable* obj = *it;
			if (!obj->isActive)
				continue;
			SDL_Rect dr = obj->draw_rect;
			if (dr.x + dr.w < 0 || dr.y + dr.h < 0)
				continue;

			SDL_manager::render_sprite(obj->sprite, dr);
		}
	}

	if(render_system::mouse != nullptr)
		if(render_system::mouse->isActive)
			SDL_manager::render_sprite(mouse->sprite, mouse->draw_rect, mouse->flipped_x, mouse->flipped_y);

	SDL_manager::end_render();
}

void render_system::set_terrain_texture(SDL_Texture* tex)
{
	asset_controller::destroy_texture(terrain_sprite);
	terrain_sprite = tex;
	terrain_prepared = true;
}

bool render_system::remove_from_queue(IDrawable* dc)
{ 
	if (dc->id < 0)
		return false;
	switch (dc->layer)
	{
	case IDrawable::layers::background:
		if (dc->id >= (int)background.size())
			break;
		background.erase(background.begin() + dc->id);
		for (IDrawable* d_c : background)
		{
			if (d_c->id < dc->id)
				continue;
			d_c->id--;
		}
		break;
	case IDrawable::layers::surface:
		if (dc->id >= (int)surface.size())
			break;
		surface.erase(surface.begin() + dc->id);
		for (IDrawable* d_c : surface)
		{
			if (d_c->id < dc->id)
				continue;
			d_c->id--;
		}
		break;
	case IDrawable::layers::foreground:
		if (dc->id >= (int)foreground.size())
			break;
		foreground.erase(foreground.begin() + dc->id);
		for (IDrawable* d_c : foreground)
		{
			if (d_c->id < dc->id)
				continue;
			d_c->id--;
		}
		break;
	case IDrawable::layers::world_ui:
		if (dc->id >= (int)world_ui.size())
			break;
		world_ui.erase(world_ui.begin() + dc->id);
		for (IDrawable* d_c : ui)
		{
			if (d_c->id < dc->id)
				continue;
			d_c->id--;
		}
		break;
	case IDrawable::layers::ui:
		if (dc->id >= (int)ui.size())
			break;
		ui.erase(ui.begin() + dc->id);
		for (IDrawable* d_c : ui)
		{
			if (d_c->id < dc->id)
				continue;
			d_c->id--;
		}
		break;
	case IDrawable::layers::mouse:
		render_system::mouse = nullptr;
		break;
	}
	return true;


}



render_system::render_system()
{

}


render_system::~render_system()
{
}
