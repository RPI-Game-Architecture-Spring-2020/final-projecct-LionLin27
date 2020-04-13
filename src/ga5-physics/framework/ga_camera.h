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

/*
** Represents the a camera used to navigate the simulation world.
*/
class ga_camera
{
public:
	// The camera assumes an initial direction along the positive z axis, and
	// up vector along the positive y.
	ga_camera(const ga_vec3f& eye);
	~ga_camera();

	void update(struct ga_frame_params* params);

	void rotate(const struct ga_quatf& rotation);

	const ga_mat4f& get_transform() const { return _transform; }
	void set_transform(const ga_mat4f& t) { _transform = t; }

private:
	ga_mat4f _transform;
};
