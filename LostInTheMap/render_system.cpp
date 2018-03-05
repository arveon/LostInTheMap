#include "render_system.h"

std::vector<IDrawable*> render_system::background;
std::vector<IDrawable*> render_system::foreground;
std::vector<IDrawable*> render_system::surface;
std::vector<IDrawable*> render_system::ui;

int render_system::add_object_to_queue(IDrawable * obj)
{
	int id = -1;
	switch (obj->layer)
	{
	case IDrawable::layers::background:
		background.push_back(obj);
		id = background.size()-1;
		break;
	case IDrawable::layers::surface:
		surface.push_back(obj);
		id = surface.size()-1;
		break;
	case IDrawable::layers::foreground:
		foreground.push_back(obj);
		id = foreground.size()-1;
		break;
	case IDrawable::layers::ui:
		ui.push_back(obj);
		id = ui.size() - 1;
		break;
	}
	return id;
}

void render_system::flush_queues()
{
	background.clear();
	surface.clear();
	foreground.clear();
	ui.clear();
}

void render_system::sort_queues()
{

}

void render_system::render_queues()
{
	SDL_manager::start_render();
	if (background.size() > 0)
	{
		for (std::vector<IDrawable*>::iterator it = background.begin(); it != background.end(); it++)
		{
			IDrawable* obj = *it;
			SDL_manager::render_sprite(obj->sprite, obj->draw_rect);
		}
	}

	if (surface.size() > 0)
	{
		for (std::vector<IDrawable*>::iterator it = surface.begin(); it != surface.end(); it++)
		{
			IDrawable* obj = *it;
			SDL_manager::render_sprite(obj->sprite, obj->draw_rect);
		}
	}

	if (foreground.size() > 0)
	{
		for (std::vector<IDrawable*>::iterator it = foreground.begin(); it != foreground.end(); it++)
		{
			IDrawable* obj = *it;
			SDL_manager::render_sprite(obj->sprite, obj->draw_rect);
		}
	}

	if (ui.size() > 0)
	{
		for (std::vector<IDrawable*>::iterator it = ui.begin(); it != ui.end(); it++)
		{
			IDrawable* obj = *it;
			SDL_manager::render_sprite(obj->sprite, obj->draw_rect);
		}
	}

	SDL_manager::end_render();
}

bool render_system::remove_from_queue(int id, IDrawable::layers layer)
{
	switch (layer)
	{
	case IDrawable::layers::background:
		background.erase(background.begin() + id);
		id = background.size() - 1;
		break;
	case IDrawable::layers::surface:
		surface.erase(surface.begin() + id);
		id = surface.size() - 1;
		break;
	case IDrawable::layers::foreground:
		foreground.erase(foreground.begin() + id);
		id = foreground.size() - 1;
		break;
	case IDrawable::layers::ui:
		ui.erase(ui.begin() + id);
		id = ui.size() - 1;
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
