/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_debug_geometry.h"

#include "framework/ga_drawcall.h"

#include "math/ga_mat4f.h"
#include "math/ga_math.h"
#include "math/ga_quatf.h"
#include "math/ga_vec4f.h"

#include <vector>

void draw_debug_sphere(float radius, const ga_mat4f& transform, ga_dynamic_drawcall* drawcall)
{
	// Represent the sphere with a circle around each axis.
	std::vector<ga_vec3f> axes;
	axes.push_back(ga_vec3f::x_vector());
	axes.push_back(ga_vec3f::y_vector());
	axes.push_back(ga_vec3f::z_vector());

	const uint32_t k_line_segments = 36;

	std::vector<ga_vec4f> starts;
	starts.push_back({ 0.0f, 0.0f, radius, 1.0f });
	starts.push_back({ 0.0f, 0.0f, radius, 1.0f });
	starts.push_back({ radius, 0.0f, 0.0f, 1.0f });

	for (uint32_t axis = 0; axis < axes.size(); ++axis)
	{
		const float rotation_radians = ga_degrees_to_radians(360.0f / float(k_line_segments));
		ga_quatf rotation_quat;
		rotation_quat.make_axis_angle(axes[axis], rotation_radians);

		ga_mat4f rotation_matrix;
		rotation_matrix.make_rotation(rotation_quat);

		ga_vec4f new_point = starts[axis];
		drawcall->_positions.push_back({ new_point.x, new_point.y, new_point.z });

		for (uint32_t i = 0; i < k_line_segments - 1; ++i)
		{
			new_point = rotation_matrix.transform(new_point);

			drawcall->_positions.push_back({ new_point.x, new_point.y, new_point.z });
			drawcall->_indices.push_back((uint16_t)(drawcall->_positions.size() - 2));
			drawcall->_indices.push_back((uint16_t)(drawcall->_positions.size() - 1));
		}

		// And close the loop.
		drawcall->_indices.push_back((uint16_t)(drawcall->_positions.size() - 1));
		drawcall->_indices.push_back((uint16_t)(drawcall->_positions.size() - k_line_segments));
	}

	drawcall->_draw_mode = GL_LINES;
	drawcall->_color = { 0.0f, 0.0f, 1.0f };
	drawcall->_transform = transform;
}
