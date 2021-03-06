/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_entity.h"
#include "ga_component.h"

// TODO: move these else where
#include "graphics/ga_model_component.h"
#include "graphics/ga_light_component.h"
#include <cstring>

ga_entity::ga_entity()
{
	_name = "default_name";
	_transform.make_identity();
	_size = {1,1,1};
}

ga_entity::ga_entity(const char* name)
{
	_name = name;
	_transform.make_identity();
	_size = {1,1,1};
}

ga_entity::~ga_entity()
{
}

void ga_entity::add_component(ga_component* comp)
{
	_components.push_back(comp);
}

void ga_entity::update(ga_frame_params* params)
{
	for (auto& c : _components)
	{
		c->update(params);
	}
}

void ga_entity::late_update(ga_frame_params* params)
{
	for (auto& c : _components)
	{
		c->late_update(params);
	}
}

void ga_entity::translate(const ga_vec3f& translation)
{
	_transform.translate(translation);
}

void ga_entity::rotate(const ga_quatf& rotation)
{
	ga_mat4f rotation_m;
	rotation_m.make_rotation(rotation);
	_transform = rotation_m * _transform;
}

void ga_entity::scale(float s)
{
	_transform.scale(s);
	_size *= {s,s,s};
}

void ga_entity::scale(ga_vec3f s)
{
	_transform.scale(s);
	_size *= s;
}

void ga_entity::set_position(const ga_vec3f& new_pos)
{
	_transform.set_translation(new_pos);
}

const char* ga_entity::get_name()
{
	return _name;
}

ga_component* ga_entity::get_component(const char* name) {
	for (auto& c : _components)
	{
		if (dynamic_cast<ga_model_component*>(c) && strcmp(name, "ga_model_component") == 0) {
			return c;
		}
		if (dynamic_cast<ga_light_component*>(c) && strcmp(name, "ga_light_component") == 0) {
			return c;
		}
	}
	return nullptr;
}
