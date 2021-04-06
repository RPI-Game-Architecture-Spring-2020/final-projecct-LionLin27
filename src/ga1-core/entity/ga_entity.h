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

#include <vector>

/*
** Entity object.
** A bucket of components in 3D space. No classes should derive from here.
** All functionality should be in components.
** @see ga_component
*/
class ga_entity final
{
public:
	ga_entity();
	ga_entity(const char* name);
	~ga_entity();

	void add_component(class ga_component* comp);

	void update(struct ga_frame_params* params);
	void late_update(struct ga_frame_params* params);

	void translate(const struct ga_vec3f& translation);
	void rotate(const struct ga_quatf& rotation);
	void scale(float s);

	void set_position(const struct ga_vec3f& new_pos);

	const ga_mat4f& get_transform() const { return _transform; }
	void set_transform(const ga_mat4f& t) { _transform = t; }

	const char* get_name();
	class ga_component* get_component(const char* name);

	float get_size() { return _size; }

private:
	std::vector<class ga_component*> _components;
	ga_mat4f _transform;
	float _size;
	const char* _name;
};
