#include "SplashScreeenSystem.h"

SplashScreenSystem::splash_state SplashScreenSystem::state;
int SplashScreenSystem::cur_alpha;
int SplashScreenSystem::elapsed_stage_time;
std::vector<void(*)()> SplashScreenSystem::listeners;

void SplashScreenSystem::init_space(Space& space)
{
	if (space.initialised)
		return;
	Entity* splash_screen = new Entity();
	const char* path = "assets/graphics/bg/splash.png";

	Transform* transform = new Transform();
	IDrawable* draw = new IDrawable();
	draw->sprite = asset_controller::load_texture(path);
	draw->type = ComponentType::Drawable;
	draw->layer = IDrawable::layers::ui;

	transform->isActive = true;
	transform->position = asset_controller::get_texture_size(draw->sprite);
	transform->position.x = 0;
	transform->position.y = 0;
	transform->type = ComponentType::Location;

	draw->draw_rect.x = 0;
	draw->draw_rect.y = 0;
	draw->draw_rect.w = transform->position.w;
	draw->draw_rect.h = transform->position.h;
	asset_controller::set_texture_alpha(draw->sprite, 0);

	splash_screen->add_component(transform);
	splash_screen->add_component(draw);
	space.objects.push_back(splash_screen);
	space.initialised = true;	

	SplashScreenSystem::elapsed_stage_time = 0;
	//for(std::vector<Entity*>::iterator it = )
}

void SplashScreenSystem::destroy_space(Space& space)
{
	space.objects.clear();
	space.initialised = false;
}



void SplashScreenSystem::update_space(Space& space, int dt)
{
	IDrawable* comp = static_cast<IDrawable*>(find_component_on_object(*(space.objects.begin()), ComponentType::Drawable));
	
	switch (state)
	{
	case splash_state::not_initialised:
		elapsed_stage_time += dt;
		if (elapsed_stage_time >= total_stage_time)
		{
			state = splash_state::appearing;
			elapsed_stage_time = 0;
		}
		break;
	case splash_state::appearing:
		elapsed_stage_time += dt;
		cur_alpha = static_cast<int>(elapsed_stage_time / (float)total_stage_time * 255);
		if (elapsed_stage_time >= total_stage_time)
		{
			state = splash_state::full;
			elapsed_stage_time = 0;
		}
		if (comp)
			asset_controller::set_texture_alpha(comp->sprite, cur_alpha);
		break;
	case splash_state::disappearing:
		elapsed_stage_time += dt;
		cur_alpha = static_cast<int>(255 - (elapsed_stage_time / (float)total_stage_time * 255));
		if (elapsed_stage_time >= total_stage_time)
		{
			state = splash_state::done;
			elapsed_stage_time = 0;
		}
		if (comp)
			asset_controller::set_texture_alpha(comp->sprite, cur_alpha);
		break;
		break;
	case splash_state::full:
		elapsed_stage_time += dt;
		if (elapsed_stage_time >= total_stage_time)
		{
			state = splash_state::disappearing;
			elapsed_stage_time = 0;
		}

		break;
	case splash_state::done:
		for (unsigned int i = 0; i < listeners.size(); i++)
		{
			void(*callback)() = listeners.at(i);
			callback();
		}

		break;
	}


}

SplashScreenSystem::SplashScreenSystem()
{
}


SplashScreenSystem::~SplashScreenSystem()
{
}
