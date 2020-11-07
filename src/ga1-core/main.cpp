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
#include "graphics/ga_procedural_model.h"
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

void process_herald_msg(ga_frame_params*, ga_sim*);

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


	/*
	// cg pyramid entity
	ga_entity* pyramid_entities[5];
	ga_pyramid_component* pyramid_components[5];
	for (int i = 0; i < 5; i++) {
		pyramid_entities[i] = new ga_entity("pyramid");
		pyramid_components[i] = new ga_pyramid_component(pyramid_entities[i]);
		//pyramid_entity->translate({ 0, 5, 0 });
		pyramid_entities[i]->set_position({ (float)i * 3 - 6, 5.0f, -10 });
		sim->add_entity(pyramid_entities[i]);
	}


	// Create an entity whose movement is driven by Lua script.
	ga_entity lua;
	lua.translate({ 0.0f, 2.0f, 1.0f });
	//ga_lua_component lua_move(&lua, "data/scripts/move.lua");
	ga_cube_component lua_model(&lua, "data/textures/rpi.png");
	sim->add_entity(&lua);
	*/


	// light ent
	ga_entity light_entity("directional light");
	ga_directional_light* light = new ga_directional_light({ 1,1,1 }, 0.7);
	ga_light_component light_component(&light_entity, light);

	// sphere model of light
	ga_model lightSphereModel;
	generate_sphere(12, &lightSphereModel);
	ga_material* sphereMat = new ga_constant_color_material();
	sphereMat->set_color({ 1,1,1 });
	ga_model_component sphere_mc2(&light_entity, &lightSphereModel, sphereMat);
	light_entity.scale(0.1f);

	ga_model arrowModel;
	generate_arrow(&arrowModel);
	ga_material* arrow_mat = new ga_constant_color_material();
	arrow_mat->set_color({ 1,1,1 });
	ga_model_component arrow_mce(&light_entity, &arrowModel, arrow_mat);

	sim->add_entity(&light_entity);

	// point light ent
	ga_entity light_entity2("point light 1");
	ga_positional_light* light2 = new ga_positional_light({ 0,1,1 }, 1);
	ga_light_component light_component2(&light_entity2, light2);

	// sphere model of light
	ga_model lightSphereModel2;
	generate_sphere(12, &lightSphereModel2);
	ga_material* sphereMat2 = new ga_constant_color_material();
	sphereMat2->set_color({ 0,1,1 });
	ga_model_component sphere_mc3(&light_entity2, &lightSphereModel2, sphereMat2);
	light_entity2.scale(0.1f);
	sim->add_entity(&light_entity2);
	//light_entity2.translate({ -3, -2, -1 });

	/*
	// point light ent
	ga_entity light_entity3("point light 2");
	ga_positional_light* light3 = new ga_positional_light({ 1,0,1 }, 1, { 1,1,1 });
	ga_light_component light_component3(&light_entity3, light3);

	// sphere model of light
	ga_model lightSphereModel3;
	generate_sphere(12, &lightSphereModel3);
	ga_material* sphereMat3 = new ga_constant_color_material();
	sphereMat3->set_color({ 1,0,1 });
	ga_model_component sphere_mc4(&light_entity3, &lightSphereModel3, sphereMat3);
	light_entity3.scale(0.1f);
	sim->add_entity(&light_entity3);
	light_entity3.translate({ -2, 1, 1 });

	// point light ent
	ga_entity light_entity4("point light 3");
	ga_positional_light* light4 = new ga_positional_light({ 1,1,0 }, 1, { 1,1,1 });
	ga_light_component light_component4(&light_entity4, light4);

	// sphere model of light
	ga_model lightSphereModel4;
	generate_sphere(12, &lightSphereModel4);
	ga_material* sphereMat4 = new ga_constant_color_material();
	sphereMat4->set_color({ 1,1,0 });
	ga_model_component sphere_mc5(&light_entity4, &lightSphereModel4, sphereMat4);
	light_entity4.scale(0.1f);
	sim->add_entity(&light_entity4);
	light_entity4.translate({ 1, 2, 1 });
	*/

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

	sim->add_entity(&animated_entity);

	animation_component.play(&animation);
	*/

	// spaceship entity
	ga_entity shipEnt("ship");
	ga_model shipModel;
	obj_to_model("data/models/shuttle.obj", &shipModel);
	ga_material* lit_mat0 = new ga_lit_material("data/textures/spstob_1.jpg");
	ga_model_component ship_model_component(&shipEnt, &shipModel, lit_mat0, true);
	sim->add_entity(&shipEnt);
	shipEnt.scale(10.0f);
	shipEnt.set_position({0, -10, 0});
	/*
	// procedual sphere
	ga_entity sphereEnt("earth");
	ga_model sphereModel;
	generate_sphere(64, &sphereModel);
	ga_model_component sphere_mc(&sphereEnt, &sphereModel, "data/textures/earth.jpg");
	ga_lua_component lua_rotate(&sphereEnt, "data/scripts/slow_rotate.lua");
	sim->add_entity(&sphereEnt);
	sphereEnt.scale(1000.0f);
	sphereEnt.set_position({0,0,-2000});
	*/


	// lit torus
	ga_entity torusEnt("torus");
	ga_model torusModel;
	generate_torus(1.5f, 0.7f, 30, &torusModel);

	ga_material* lit_mat = new ga_lit_material("data/textures/checker.png");

	ga_model_component torus_mc(&torusEnt, &torusModel, lit_mat, true);
	sim->add_entity(&torusEnt);
	torusEnt.translate({ 0,-3,0 });

	// lit sphere
	ga_entity sphereEnt("sphere");
	ga_model sphereModel;
	generate_sphere(30, &sphereModel);

	ga_material* lit_mat2 = new ga_lit_material("data/textures/checker.png");

	ga_model_component sphere_mce(&sphereEnt, &sphereModel, lit_mat2, true);
	sim->add_entity(&sphereEnt);
	sphereEnt.translate({ 0,2,0 });

	/*
	*/
	// lit plane
	ga_entity planeEnt("plane");
	ga_model planeModel;
	generate_plane(&planeModel);
	ga_material* plane_mat = new ga_lit_material("data/textures/checker.png");
	ga_model_component plane_mce(&planeEnt, &planeModel, plane_mat, true);
	sim->add_entity(&planeEnt);
	planeEnt.translate({0,-0.1,0});
	planeEnt.scale(100);



	ga_frame_herald herald;

	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// get information from last frame from the herald
		params._herald = &herald;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

		// do stuff with info from last frame
		// TODO: distribute jobs to different components
		process_herald_msg(&params, sim);


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


void create_sphere(ga_sim* sim) {
	// procedual sphere
	ga_entity* sphereEnt = new ga_entity("shpere");
	ga_model* sphereModel = new ga_model();
	generate_sphere(64, sphereModel);
	ga_model_component* sphere_mc = new ga_model_component(sphereEnt, sphereModel, "data/textures/earth.jpg");
	//ga_lua_component lua_rotate(&sphereEnt, "data/scripts/slow_rotate.lua");
	sim->add_entity(sphereEnt);
}

void process_herald_msg(ga_frame_params* params, ga_sim* sim) {
	if (params->_herald->_create_sphere) {
		create_sphere(sim);
		params->_herald->_create_sphere = false;
	}
}


