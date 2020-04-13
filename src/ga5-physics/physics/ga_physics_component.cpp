/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_physics_component.h"
#include "ga_physics_world.h"
#include "ga_rigid_body.h"

#include "entity/ga_entity.h"

ga_physics_component::ga_physics_component(ga_entity* ent, ga_shape* shape, float mass)
	: ga_component(ent)
{
	_body = new ga_rigid_body(shape, mass);
	_body->_transform = ent->get_transform();
}

ga_physics_component::~ga_physics_component()
{
	delete _body;
}

void ga_physics_component::update(ga_frame_params* params)
{
	// First, re-sync the rigid body's transform with the entity's.
	_body->_transform = get_entity()->get_transform();

#if GA_PHYSICS_DEBUG_DRAW
	ga_dynamic_drawcall draw;
	_body->get_debug_draw(&draw);

	while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_dynamic_drawcalls.push_back(draw);
	params->_dynamic_drawcall_lock.clear(std::memory_order_release);
#endif
}

void ga_physics_component::late_update(ga_frame_params* params)
{
	// Sync the entity's transform with the rigid body's.
	get_entity()->set_transform(_body->_transform);
}
