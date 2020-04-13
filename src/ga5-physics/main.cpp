/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_camera.h"
#include "framework/ga_compiler_defines.h"
#include "framework/ga_input.h"
#include "framework/ga_sim.h"
#include "framework/ga_output.h"
#include "jobs/ga_job.h"

#include "entity/ga_entity.h"

#include "graphics/ga_cube_component.h"
#include "graphics/ga_program.h"

#include "gui/ga_font.h"

#include "physics/ga_intersection.tests.h"
#include "physics/ga_physics_component.h"
#include "physics/ga_physics_world.h"
#include "physics/ga_rigid_body.h"
#include "physics/ga_shape.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

ga_font* g_font = nullptr;

static void set_root_path(const char* exepath);
static void run_unit_tests();

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	run_unit_tests();

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_physics_world* world = new ga_physics_world();
	ga_output* output = new ga_output(input->get_window());

	// Create camera.
	ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// First text case: A single box with no rotation falling to the floor.
	ga_entity test_1_box;
	test_1_box.translate({ -6.0f, 8.0f, 0.0f });

	ga_oobb test_1_oobb;
	test_1_oobb._half_vectors[0] = ga_vec3f::x_vector();
	test_1_oobb._half_vectors[1] = ga_vec3f::y_vector();
	test_1_oobb._half_vectors[2] = ga_vec3f::z_vector();

	ga_physics_component test_1_collider(&test_1_box, &test_1_oobb, 1.0f);

	world->add_rigid_body(test_1_collider.get_rigid_body());
	sim->add_entity(&test_1_box);

	// Second test case: Two boxes with no rotation falling on top of each other.
	ga_entity test_2_box_1;
	test_2_box_1.translate({ 0.0f, 4.0f, 0.0f });
	ga_entity test_2_box_2;
	test_2_box_2.translate({ 0.0f, 8.0f, 0.0f });

	ga_oobb test_2_oobb_1;
	test_2_oobb_1._half_vectors[0] = ga_vec3f::x_vector();
	test_2_oobb_1._half_vectors[1] = ga_vec3f::y_vector();
	test_2_oobb_1._half_vectors[2] = ga_vec3f::z_vector();

	ga_oobb test_2_oobb_2;
	test_2_oobb_2._half_vectors[0] = ga_vec3f::x_vector();
	test_2_oobb_2._half_vectors[1] = ga_vec3f::y_vector();
	test_2_oobb_2._half_vectors[2] = ga_vec3f::z_vector();

	ga_physics_component test_2_collider_1(&test_2_box_1, &test_2_oobb_1, 3.0f);
	ga_physics_component test_2_collider_2(&test_2_box_2, &test_2_oobb_2, 1.0f);

	world->add_rigid_body(test_2_collider_1.get_rigid_body());
	world->add_rigid_body(test_2_collider_2.get_rigid_body());
	sim->add_entity(&test_2_box_1);
	sim->add_entity(&test_2_box_2);

	// Third test case: Two weightless boxes colliding off-center.
	ga_entity test_3_box_1;
	test_3_box_1.translate({ 5.6f, 4.0f, 0.2f });
	ga_entity test_3_box_2;
	test_3_box_2.translate({ 6.5f, 8.0f, 0.0f });

	ga_oobb test_3_oobb_1;
	test_3_oobb_1._half_vectors[0] = ga_vec3f::x_vector();
	test_3_oobb_1._half_vectors[1] = ga_vec3f::y_vector().scale_result(0.7f);
	test_3_oobb_1._half_vectors[2] = ga_vec3f::z_vector().scale_result(0.8f);

	ga_oobb test_3_oobb_2;
	test_3_oobb_2._half_vectors[0] = ga_vec3f::x_vector().scale_result(0.6f);
	test_3_oobb_2._half_vectors[1] = ga_vec3f::y_vector().scale_result(0.9f);
	test_3_oobb_2._half_vectors[2] = ga_vec3f::z_vector().scale_result(0.7f);

	ga_physics_component test_3_collider_1(&test_3_box_1, &test_3_oobb_1, 2.0f);
	ga_physics_component test_3_collider_2(&test_3_box_2, &test_3_oobb_2, 3.0f);
	test_3_collider_1.get_rigid_body()->make_weightless();
	test_3_collider_2.get_rigid_body()->make_weightless();

	test_3_collider_1.get_rigid_body()->add_linear_velocity({ 0.0f, 2.0f, 0.0f });
	test_3_collider_1.get_rigid_body()->add_angular_momentum({ 0.2f, 0.3f, 0.1f });
	test_3_collider_2.get_rigid_body()->add_linear_velocity({ 0.0f, -3.0f, 0.0f });

	world->add_rigid_body(test_3_collider_1.get_rigid_body());
	world->add_rigid_body(test_3_collider_2.get_rigid_body());
	sim->add_entity(&test_3_box_1);
	sim->add_entity(&test_3_box_2);

	ga_entity floor;
	ga_plane floor_plane;
	floor_plane._point = { 0.0f, 0.0f, 0.0f };
	floor_plane._normal = { 0.0f, 1.0f, 0.0f };
	ga_physics_component floor_collider(&floor, &floor_plane, 0.0f);
	floor_collider.get_rigid_body()->make_static();
	world->add_rigid_body(floor_collider.get_rigid_body());
	sim->add_entity(&floor);

	// Create the default font:
	g_font = new ga_font("VeraMono.ttf", 16.0f, 512, 512);

	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// Step the physics world.
		world->step(&params);

		// Perform the late update.
		sim->late_update(&params);

		// Draw to screen.
		output->update(&params);
	}

	world->remove_rigid_body(floor_collider.get_rigid_body());
	world->remove_rigid_body(test_3_collider_2.get_rigid_body());
	world->remove_rigid_body(test_3_collider_1.get_rigid_body());
	world->remove_rigid_body(test_2_collider_2.get_rigid_body());
	world->remove_rigid_body(test_2_collider_1.get_rigid_body());
	world->remove_rigid_body(test_1_collider.get_rigid_body());

	delete output;
	delete world;
	delete sim;
	delete input;
	delete camera;

	ga_job::shutdown();

	return 0;
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(GA_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(GA_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}

void run_unit_tests()
{
	ga_intersection_utility_unit_tests();
	ga_intersection_unit_tests();
}
