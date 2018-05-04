#pragma once
#include <map>

#include "Entity.h"
#include "SpaceSystem.h"

enum animations
{
	idle,
	walking_right,
	walking_left,
	walking_down,
	walking_up,
	attacking_right,
	attacking_left,
	taking_damage,
	death,
	selected
};

class animator
{
private:
	static std::map<IAnimatable*, void(*)(Entity* source)> animation_done_callbacks;
	
	static const int max_frames_in_animation = 6;
public:
	static void start_animation(IAnimatable*, animations, void(*done_callback)(Entity* source)=nullptr);
	static void apply_animation_sprite_changes(Space & space);
	static void set_walking_animation(Entity * ent, ITerrain* tc);
	static void update(Space& game_space, int dt);

	animator();
	~animator();
};

