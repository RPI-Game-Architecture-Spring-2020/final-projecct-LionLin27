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
#include "entity/ga_lua_component.h"

#include "graphics/ga_cube_component.h"
#include "graphics/ga_program.h"

#include "graphics/ga_animation.h"
#include "graphics/ga_animation_component.h"
#include "graphics/ga_light_component.h"
#include "graphics/ga_egg_parser.h"
#include "graphics/ga_obj_parser.h"
#include "graphics/ga_material.h"
#include "graphics/ga_model_component.h"
#include "graphics/ga_geometry.h"
#include "graphics/ga_pyramid_component.h"

#include "gui/ga_font.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

ga_font* g_font = nullptr;
static void set_root_path(const char* exepath);

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_output* output = new ga_output(input->get_window());

	// Create the default font:
	g_font = new ga_font("VeraMono.ttf", 16.0f, 512, 512);

	// Create camera.
	ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);
	

	// cg pyramid entity
	ga_entity pyramid_entity;
	ga_pyramid_component pyramid_component(&pyramid_entity);
	pyramid_entity.translate({ 1, 0, 0 });
	sim->add_entity(&pyramid_entity);



	// Create an entity whose movement is driven by Lua script.
	ga_entity lua;
	lua.translate({ 0.0f, 2.0f, 1.0f });
	//ga_lua_component lua_move(&lua, "data/scripts/move.lua");
	ga_cube_component lua_model(&lua, "data/textures/rpi.png");
	sim->add_entity(&lua);
	

	// light
	ga_entity light_entity;
	ga_directional_light* light = new ga_directional_light({ 1,1,1 }, 1, { 1,1,1 });
	ga_light_component light_component(&light_entity, light);
	sim->add_entity(&light_entity);


	/*
	// Create an animated entity.
	ga_model animated_model;
	egg_to_model("data/models/bar.egg", &animated_model);

	ga_animation animation;
	egg_to_animation("data/animations/bar_bend.egg", &animation);

	ga_entity animated_entity;

	// model & anim
	//ga_lit_anim_material* animated_material = new ga_lit_anim_material(animated_model._skeleton);
	ga_model_component model_component(&animated_entity, &animated_model, light);
	ga_animation_component animation_component(&animated_entity, &animated_model);

	// rotate & move
	//ga_lua_component lua_rotate(&animated_entity, "data/scripts/rotate.lua");
	//ga_lua_component lua_move(&animated_entity, "data/scripts/move.lua");

	//sim->add_entity(&animated_entity);

	animation_component.play(&animation);
	*/

	
	// spaceship entity
	ga_entity shipEnt;
	ga_model shipModel;
	obj_to_model("data/models/shuttle.obj", &shipModel);
	ga_model_component ship_model_component(&shipEnt, &shipModel, "data/textures/spstob_1.jpg");
	sim->add_entity(&shipEnt);

	
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

		// Perform the late update.
		sim->late_update(&params);

		// Draw to screen.
		output->update(&params);
	}

	delete output;
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
