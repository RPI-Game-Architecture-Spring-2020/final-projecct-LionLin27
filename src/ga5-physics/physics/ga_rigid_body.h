#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_mat4f.h"
#include "math/ga_vec3f.h"

#include <cstdint>
#include <vector>

enum ga_rigid_body_flags
{
	k_static = 1,
	k_weightless = 2,
};

/*
** Represents a body in the physics simulation.
** Static bodies will not move (e.g. the floor).
*/
class ga_rigid_body final
{
public:
	ga_rigid_body(struct ga_shape* shape, float mass);
	~ga_rigid_body();

	void get_debug_draw(struct ga_dynamic_drawcall* drawcall);

	void make_static();
	void make_weightless();

	void add_linear_velocity(const ga_vec3f& v);
	void add_angular_momentum(const ga_vec3f& v);

private:
	ga_mat4f _transform;
	ga_quatf _orientation = { 0.0f, 0.0f, 0.0f, 0.0f };

	ga_vec3f _angular_momentum = ga_vec3f::zero_vector();
	ga_vec3f _angular_velocity = ga_vec3f::zero_vector();
	ga_vec3f _velocity = ga_vec3f::zero_vector();

	ga_mat4f _inertia_tensor;

	float _mass;

	// Ordinarily this would live in a collision material structure.
	// We just include the value here for simplicity.
	float _coefficient_of_restitution = 0.5f;

	struct ga_shape* _shape;

	std::vector<ga_vec3f> _forces;
	std::vector<ga_vec3f> _torques;

	uint32_t _flags;

	friend class ga_physics_world;
	friend class ga_physics_component;
};
