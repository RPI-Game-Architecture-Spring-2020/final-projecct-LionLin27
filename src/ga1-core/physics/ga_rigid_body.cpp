/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_rigid_body.h"
#include "ga_shape.h"

ga_rigid_body::ga_rigid_body(ga_shape* shape, float mass) : _mass(mass), _shape(shape), _flags(0)
{
	_transform.make_identity();
	_orientation.make_axis_angle(ga_vec3f::y_vector(), 0);

	_shape->get_inertia_tensor(_inertia_tensor, _mass);
}

ga_rigid_body::~ga_rigid_body()
{
}

void ga_rigid_body::get_debug_draw(ga_dynamic_drawcall* drawcall)
{
	_shape->get_debug_draw(_transform, drawcall);
}

void ga_rigid_body::make_static()
{
	_flags |= k_static;
}

void ga_rigid_body::make_weightless()
{
	_flags |= k_weightless;
}

void ga_rigid_body::add_linear_velocity(const ga_vec3f& v)
{
	_velocity += v;
}

void ga_rigid_body::add_angular_momentum(const ga_vec3f& v)
{
	_angular_momentum += v;
}
