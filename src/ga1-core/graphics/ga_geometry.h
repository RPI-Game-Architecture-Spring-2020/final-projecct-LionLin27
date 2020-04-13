#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <climits>
#include <cstdint>
#include <vector>

#include "math/ga_mat4f.h"
#include "math/ga_vec2f.h"
#include "math/ga_vec3f.h"

struct ga_vertex
{
	static const uint32_t k_max_joint_weights = 4;

	ga_vec3f _position;
	ga_vec3f _normal;
	ga_vec3f _color;
	ga_vec2f _uv;

	uint32_t _joints[4] = { 0, 0, 0, 0 };
	float _weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

enum ga_vertex_attribute
{
	k_vertex_attribute_normal = 1,
	k_vertex_attribute_color = 2,
	k_vertex_attribute_uv = 4,
	k_vertex_attribute_weight = 8,
};

struct ga_model
{
	ga_model();
	~ga_model();

	uint32_t _vertex_format = 0;

	std::vector<ga_vertex> _vertices;
	std::vector<uint16_t> _indices;
	const char* _texture_name;

	struct ga_skeleton* _skeleton;
};
