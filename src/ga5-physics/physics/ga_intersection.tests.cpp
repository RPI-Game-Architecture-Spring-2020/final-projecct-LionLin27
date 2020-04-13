/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_intersection.tests.h"
#include "ga_intersection.h"

#include "ga_shape.h"

#include <cassert>

void ga_intersection_utility_unit_tests()
{
	// Test line to line points.
	{
		ga_vec3f start_a = { -1.0f, 1.0f, 0.0f };
		ga_vec3f end_a = { 1.0f, 1.0f, 0.0f };
		ga_vec3f start_b = { 0.0f, 0.0f, 1.0f };
		ga_vec3f end_b = { 0.0f, 0.0f, -1.0f };
		ga_vec3f point_a;
		ga_vec3f point_b;

		closest_points_on_lines(start_a, end_a, start_b, end_b, point_a, point_b);

		assert(point_a.equal({ 0.0f, 1.0f, 0.0f }));
		assert(point_b.equal({ 0.0f, 0.0f, 0.0f }));
	}
}

void ga_intersection_unit_tests()
{
	// Test OOBB collisions.
	{
		ga_oobb oobb_a, oobb_b;
		oobb_a._center = { 0.0f, 0.0f, 0.0f };
		oobb_a._half_vectors[0] = { 0.6f, 0.0f, 0.0f };
		oobb_a._half_vectors[1] = { 0.0f, 1.0f, 0.0f };
		oobb_a._half_vectors[2] = { 0.0f, 0.0f, 0.3f };
		oobb_b._center = { 0.0f, 0.0f, 0.0f };
		oobb_b._half_vectors[0] = { 1.0f, 0.0f, 0.0f };
		oobb_b._half_vectors[1] = { 0.0f, 1.0f, 0.0f };
		oobb_b._half_vectors[2] = { 0.0f, 0.0f, 1.0f };

		ga_mat4f trans_a, trans_b;
		trans_a.make_identity();
		trans_b.make_translation({ -1.0f, 0.0f, 0.0f });

		ga_collision_info info;
		bool collision = separating_axis_test(&oobb_a, trans_a, &oobb_b, trans_b, &info);
		assert(collision);

		ga_quatf rotation_q;
		rotation_q.make_axis_angle({ 0.0f, 0.0f, 1.0f }, ga_degrees_to_radians(45.0f));
		trans_b.rotate(rotation_q);

		collision = separating_axis_test(&oobb_a, trans_a, &oobb_b, trans_b, &info);
		assert(collision);

		rotation_q.make_axis_angle({ 1.0f, 0.0f, 0.0f }, ga_degrees_to_radians(90.0f));
		trans_b.rotate(rotation_q);

		collision = separating_axis_test(&oobb_a, trans_a, &oobb_b, trans_b, &info);
		assert(collision);

		trans_a.translate({ 0.0f, -5.0f, 0.0f});

		collision = separating_axis_test(&oobb_a, trans_a, &oobb_b, trans_b, &info);
		assert(!collision);
	}
}
