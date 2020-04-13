#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec3f.h"

#include <atomic>
#include <vector>

#define GA_PHYSICS_DEBUG_DRAW 1

struct ga_collision_info;
class ga_rigid_body;
struct ga_frame_params;

/*
** Represents the physics simulation environment.
** Tracks all rigid bodies and dispatches the physics and collision simulations.
*/
class ga_physics_world
{
public:
	ga_physics_world();
	~ga_physics_world();

	void add_rigid_body(ga_rigid_body* body);
	void remove_rigid_body(ga_rigid_body* body);

	void step(ga_frame_params* params);

private:
	std::vector<ga_rigid_body*> _bodies;
	std::atomic_flag _bodies_lock = ATOMIC_FLAG_INIT;

	ga_vec3f _gravity;

	void step_linear_dynamics(ga_frame_params* params, ga_rigid_body* body);
	void step_angular_dynamics(ga_frame_params* params, ga_rigid_body* body);

	void test_intersections(ga_frame_params* params);

	void resolve_collision(ga_rigid_body* body_a, ga_rigid_body* body_b, ga_collision_info* info);
};
