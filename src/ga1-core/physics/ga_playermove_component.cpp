/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_playermove_component.h"

#include "entity/ga_entity.h"

ga_playermove_component::ga_playermove_component(ga_entity* ent)
	: ga_component(ent), _move_when_paused(false)
{
}

ga_playermove_component::~ga_playermove_component()
{
}

void ga_playermove_component::update(ga_frame_params* params)
{
	float dt = _move_when_paused ?
		1.0f / 60.0f :
		std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();

	if (params->_button_mask & k_button_j)
	{
		get_entity()->translate({ -5.0f * dt, 0.0f, 0.0f });
	}
	if (params->_button_mask & k_button_l)
	{
		get_entity()->translate({ 5.0f * dt, 0.0f, 0.0f });
	}
	if (params->_button_mask & k_button_i)
	{
		get_entity()->translate({ 0.0f, 5.0f * dt, 0.0f });
	}
	if (params->_button_mask & k_button_k)
	{
		get_entity()->translate({ 0.0f, -5.0f * dt, 0.0f });
	}
	if (params->_button_mask & k_button_o)
	{
		ga_vec3f axis = { 1.0f, 1.0f, 1.0f };
		ga_quatf axis_angle;
		axis_angle.make_axis_angle(axis, ga_degrees_to_radians(180.0f * dt));
		get_entity()->rotate(axis_angle);
	}
}

void ga_playermove_component::set_move_when_paused(bool state)
{
	_move_when_paused = state;
}
