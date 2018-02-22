#include "SplashScreeenSystem.h"



void SplashScreenSystem::init_space(Space& space)
{
	Entity* splash_screen = new Entity();
	const char* path = "assets/bg/splash.png";

	Transform* transform = new Transform();
	IDrawable* draw = new IDrawable();
	draw->sprite = asset_controller::load_texture(path);
	
	transform->isActive = true;
	transform->position = asset_controller::get_texture_size(draw->sprite);
	transform->position.x = 0;
	transform->position.y = 0;

	splash_screen->add_component(transform);
	splash_screen->add_component(draw);

	space.objects.push_back(new Entity());


}

SplashScreenSystem::SplashScreenSystem()
{
}


SplashScreenSystem::~SplashScreenSystem()
{
}
