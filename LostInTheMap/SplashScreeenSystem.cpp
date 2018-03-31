#include "SplashScreeenSystem.h"

SplashScreenSystem::splash_state SplashScreenSystem::state;
int SplashScreenSystem::cur_alpha;
int SplashScreenSystem::elapsed_stage_time;
std::vector<void(*)()> SplashScreenSystem::listeners;

void SplashScreenSystem::init_space(Space& space)
{
	if (space.initialised)
		return;
	Entity* splash_screen = new Entity(entity_type::background);
	const char* path = "assets/graphics/bg/splash.png";

	Transform* transform = new Transform(splash_screen);
	IDrawable* draw = new IDrawable(splash_screen, IDrawable::layers::background);
	draw->sprite = asset_controller::load_texture(path);

	transform->position = asset_controller::get_texture_size(draw->sprite);


	draw->draw_rect = { 0,0,transform->position.w,transform->position.h };
	asset_controller::set_texture_alpha(draw->sprite, 0);

	splash_screen->add_component(transform);
	splash_screen->add_component(draw);
	space.objects.push_back(splash_screen);
	space.initialised = true;	

	SplashScreenSystem::elapsed_stage_time = 0;
	//for(std::vector<Entity*>::iterator it = )
}



void SplashScreenSystem::update_space(Space& space, int dt)
{
	if (!space.initialised)
		return;

	Entity* first = *(space.objects.begin());
	IDrawable* comp = static_cast<IDrawable*>(first->get_component(Component::ComponentType::Drawable));
	
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
