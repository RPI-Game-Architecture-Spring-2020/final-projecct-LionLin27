/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <iostream>

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
#include "graphics/ga_assimp_loader.h"
#include "graphics/ga_procedural_model.h"
#include "graphics/ga_material.h"
#include "graphics/ga_model_component.h"
#include "graphics/ga_geometry.h"
#include "graphics/ga_pyramid_component.h"
#include "graphics/ga_skybox.h"

#include "gui/ga_font.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>


#if defined(GA_MINGW)
#include <unistd.h>
#endif
#include <graphics/ga_interiorDistanceScanner.h>

// bunny is too expensive to load everytime
ga_model* bunny_model;

struct TexturePack {
	const char* texture;
	const char* normal;
	const char* roughness;
	const char* metallic;

	float roughness_f;
	float metalness_f;

	TexturePack(const char* txt, const char* nml, const char* rgh, const char* mtl, float rf, float mf) : 
		texture(txt), normal(nml), roughness(rgh), metallic(mtl),roughness_f(rf), metalness_f(mf) {};
};

TexturePack my_test_pack("data/textures/checker.png", "data/textures/lego_normal.png", "", "", 0.1f, 0.5f);
TexturePack marble_pack("data/textures/Marble6/Color.jpg", "data/textures/Marble6/Normal.jpg", "data/textures/Marble6/Roughness.jpg", "", 0.1f, 0.1f);
TexturePack rustiron_pack("data/textures/rustediron/basecolor.png", "data/textures/rustediron/normal.png", "data/textures/rustediron/roughness.png", "data/textures/rustediron/metallic.png", 0.9f, 0.6f);
TexturePack steel_pack("data/textures/Metal38/Color.jpg", "data/textures/Metal38/Normal.jpg", "data/textures/Metal38/Roughness.jpg", "data/textures/Metal38/Metalness.jpg", 0.4f, 0.7f);
TexturePack wood_pack("data/textures/Wood47/Color.jpg", "data/textures/Wood47/Normal.jpg", "data/textures/Wood47/Roughness.jpg", "", 1.0f, 0.0f);
TexturePack tile_pack("data/textures/Tiles83/Color.jpg", "data/textures/Tiles83/Normal.jpg", "data/textures/Tiles83/Roughness.jpg", "", 0.8f, 0.0f);
TexturePack moon_pack{ "data/textures/moon.jpg", "data/textures/moonNormal.jpg", "", "", 1.0f, 0.0f };

TexturePack tparr[] = { my_test_pack, marble_pack, rustiron_pack, steel_pack, wood_pack, tile_pack, moon_pack };

ga_font* g_font = nullptr;
static void set_root_path(const char* exepath);

void process_herald_msg(ga_frame_params*, ga_sim*);
ga_entity* create_sphere(ga_sim* sim, ga_vec3f pos, ga_material* mat);

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
	ga_camera* camera = new ga_camera({ 0.0f, 0.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// skybox
	ga_cube_texture sky_tex("data/textures/sybox_forest_pine");
	//ga_cube_texture sky_tex("data/textures/cubeMap");
	ga_skybox skybox(&sky_tex);
	output->SetSkybox(&skybox);
	skybox.init();

	sim->set_env_map(&sky_tex);

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

	ga_model arrowModel;
	generate_arrow(&arrowModel);
	ga_material* arrow_mat = new ga_constant_color_material();
	arrow_mat->set_color({ 1,1,1 });
	ga_model_component arrow_mce(&light_entity, &arrowModel, arrow_mat);
	
	/*
	*/
	ga_quatf rot;
	rot.make_axis_angle({ 1, 0,0 }, -45);
	light_entity.scale(0.1f);
	light_entity.rotate(rot);

	sim->add_entity(&light_entity);

	/*
	// point light ent
	ga_entity light_entity2("point light 1");
	ga_positional_light* light2 = new ga_positional_light({ 1,1,1 }, 1);
	ga_light_component light_component2(&light_entity2, light2);

	// sphere model of light
	ga_model lightSphereModel2;
	generate_sphere(12, &lightSphereModel2);
	ga_material* sphereMat2 = new ga_constant_color_material();
	sphereMat2->set_color({ 1,1,1 });
	ga_model_component sphere_mc3(&light_entity2, &lightSphereModel2, sphereMat2);
	light_entity2.scale(0.1f);
	sim->add_entity(&light_entity2);
	light_entity2.translate({ 15, -5, -1 });


	// refractive torus
	ga_entity torusRefEnt2("sphere");
	ga_model torusRefModel2;
	//generate_torus(1.5f, 0.7f, 30, &torusRefModel2);
	generate_sphere(30, &torusRefModel2);
	computeInteriorDistances(&torusRefModel2);

	ga_material* refr_mat = new ga_refractive_lit_material("data/textures/checker.png", "data/textures/lego_normal.png", &sky_tex, "data/textures/RocketTextures/Roughness.png", "data/textures/RocketTextures/Metallic.png");
	ga_model_component torus_refr_mc(&torusRefEnt2, &torusRefModel2, refr_mat, true);
	sim->add_entity(&torusRefEnt2);
	torusRefEnt2.translate({ -10,-7,10 });


	// point light ent
	ga_entity light_entity3("point light 2");
	ga_positional_light* light3 = new ga_positional_light({ 1,0,1 }, 1);
	ga_light_component light_component3(&light_entity3, light3);

	// sphere model of light
	ga_model lightSphereModel3;
	generate_sphere(12, &lightSphereModel3);
	ga_material* sphereMat3 = new ga_constant_color_material();
	sphereMat3->set_color({ 1,0,1 });
	ga_model_component sphere_mc4(&light_entity3, &lightSphereModel3, sphereMat3);
	light_entity3.scale(0.1f);
	sim->add_entity(&light_entity3);
	light_entity3.translate({ -2,-5, 1 });
	// point light ent
	ga_entity light_entity4("point light 3");
	ga_positional_light* light4 = new ga_positional_light({ 1,1,0 }, 0.3);
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

	/*
	// spaceship entity
	ga_entity shipEnt("ship");
	ga_model shipModel;
	assimp_load_model("data/models/shuttle.obj", &shipModel);
	ga_material* lit_mat0 = new ga_reflective_lit_material("data/textures/spstob_1.jpg", "data/textures/lego_normal.png", &sky_tex);
	ga_model_component ship_model_component(&shipEnt, &shipModel, lit_mat0, true);
	sim->add_entity(&shipEnt);
	shipEnt.scale(10.0f);
	shipEnt.set_position({10, -6, 0});
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
	/*
	// lit torus
	ga_entity torusEnt("torus");
	ga_model torusModel;
	generate_torus(1.5f, 0.7f, 30, &torusModel);

	ga_material* lit_mat = new ga_lit_material("data/textures/checker.png");

	ga_model_component torus_mc(&torusEnt, &torusModel, lit_mat, true);
	sim->add_entity(&torusEnt);
	torusEnt.translate({ 5,-7,0 });

	// lit sphere
	ga_entity sphereEnt("sphere_big");
	ga_model sphereModel;
	generate_sphere(30, &sphereModel);

	ga_material* lit_mat2 = new ga_lit_material("data/textures/moon.jpg", "data/textures/moonNormal.jpg");

	ga_model_component sphere_mce(&sphereEnt, &sphereModel, lit_mat2, true);
	sim->add_entity(&sphereEnt);
	sphereEnt.translate({ 0,-1,-100 });
	sphereEnt.scale(15);

	ga_entity sphereEnt3("sphere_small");
	ga_model sphereModel3;
	generate_sphere(30, &sphereModel3);

	ga_material* sp_mat3 = new ga_lit_material("data/textures/checker.png");

	ga_model_component sphere_mce3(&sphereEnt3, &sphereModel3, sp_mat3, true);
	sim->add_entity(&sphereEnt3);
	sphereEnt3.translate({ 0, 2,-13 });

	// reflective torus
	ga_entity torusRefEnt("torus");
	ga_model torusRefModel;
	generate_torus(1.5f, 0.7f, 30, &torusRefModel);

	ga_material* ref_mat = new ga_reflective_lit_material("data/textures/checker.png", "data/textures/lego_normal.png", &sky_tex);

	ga_model_component torus_ref_mc(&torusRefEnt, &torusRefModel, ref_mat, true);
	sim->add_entity(&torusRefEnt);
	torusRefEnt.translate({ 5,-7,10 });

	// rocket
	ga_entity rocketEnt("rocket");
	ga_model rocketModel;
	assimp_load_model("data/models/TheRocket.obj", &rocketModel);
	ga_material* rocket_mat = new ga_reflective_lit_material("data/textures/RocketTextures/BaseColor.png", "data/textures/RocketTextures/Normal.png", &sky_tex, "data/textures/RocketTextures/Roughness.png", "data/textures/RocketTextures/Metallic.png", "data/textures/RocketTextures/AO.png");
	((ga_reflective_lit_material*)rocket_mat)->set_roughness(1.0);
	ga_model_component rocket_model_component(&rocketEnt, &rocketModel, rocket_mat, true);
	sim->add_entity(&rocketEnt);
	rocketEnt.scale(1.0f);
	rocketEnt.set_position({ 0, -3, 10 });
	*/

	/*
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 1; j++) {
			ga_material* mat;

			TexturePack tpp = tparr[i];

			mat = new ga_reflective_lit_material(
				tpp.texture,
				tpp.normal,
				sim->get_env_map(),
				tpp.roughness,
				tpp.metallic
			);
			((ga_reflective_lit_material*)mat)->set_roughness(tpp.roughness_f);
			((ga_reflective_lit_material*)mat)->set_metalness(tpp.metalness_f);

			ga_entity* test_sph =  create_sphere(sim, { 4.0f * i, 2.0f * j, 0.0f }, mat);
			ga_lua_component* lua_rotate = new ga_lua_component(test_sph, "data/scripts/rotate.lua");

			dynamic_cast<ga_reflective_lit_material*>(mat)->set_roughness(0.2f * (j+0.5f));
			dynamic_cast<ga_reflective_lit_material*>(mat)->set_metalness(0.2f * (i+0.5f));
			dynamic_cast<ga_reflective_lit_material*>(mat)->set_useNormalMap(false);
			dynamic_cast<ga_reflective_lit_material*>(mat)->set_debug_uniform(1);
			dynamic_cast<ga_reflective_lit_material*>(mat)->switch_brdf_comp(false, false, true);
		}
	}
	*/

	/*
	// lit plane
	ga_entity planeEnt("plane");
	ga_model planeModel;
	generate_plane(&planeModel);
	ga_material* plane_mat = new ga_lit_material("data/textures/checker.png");
	ga_model_component plane_mce(&planeEnt, &planeModel, plane_mat, true);
	sim->add_entity(&planeEnt);
	planeEnt.translate({0,-0.1,0});
	planeEnt.scale(100);

	ga_entity tess_plane_ent("tess_plane");
	ga_patch tess_plane_patch(60);

	ga_material* tess_mat = new ga_tess_plane_material("data/textures/checker.png", "data/textures/checker.png");
	ga_model_component tess_plane_mc(&tess_plane_ent, &tess_plane_patch, tess_mat, false);
	sim->add_entity(&tess_plane_ent);
	tess_plane_ent.translate({ 0,-50,0 });
	tess_plane_ent.scale(100);
	*/

	/*
	ga_entity moon_terrain_ent("moon_terrain");
	ga_terrain moon_terrain;
	ga_material* moon_terrain_mat = new ga_terrain_material("data/textures/squareMoonMap.jpg", "data/textures/squareMoonBump.jpg", "data/textures/squareMoonNormal.jpg");
	ga_model_component moon_terrain_mc(&moon_terrain_ent, &moon_terrain, moon_terrain_mat, true);
	sim->add_entity(&moon_terrain_ent);
	moon_terrain_ent.scale(1000.0);
	moon_terrain_ent.translate({ 0, -10, 0 });
	*/

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


ga_entity* create_sphere(ga_sim* sim, ga_vec3f pos, ga_material* mat) {
	// procedual sphere
	ga_entity* sphereEnt = new ga_entity("sphhere");
	ga_model* sphereModel = new ga_model();
	generate_sphere(32, sphereModel);

	if (dynamic_cast<ga_refractive_lit_material*>(mat)) {
		std::cout << "Computing interior distance of sphere." << std::endl;
		computeInteriorDistances(sphereModel);
	}

	ga_model_component* sphere_mce = new ga_model_component(sphereEnt, sphereModel, mat, true);
	//ga_lua_component lua_rotate(&sphereEnt, "data/scripts/slow_rotate.lua");
	sim->add_entity(sphereEnt);

	sphereEnt->set_position(pos);

	return sphereEnt;
}

ga_entity* create_cube(ga_sim* sim, ga_vec3f pos, ga_material* mat) {
	ga_entity* ent = new ga_entity("cube");
	ga_model* model = new ga_model();
	generate_cube(model);

	if (dynamic_cast<ga_refractive_lit_material*>(mat))
		computeInteriorDistances(model);


	ga_model_component* mce = new ga_model_component(ent, model, mat, true);
	sim->add_entity(ent);

	ent->set_position(pos);

	return ent;
}

ga_entity* create_torus(ga_sim* sim, ga_vec3f pos, ga_material* mat) {
	// procedual sphere
	ga_entity* torusEnt = new ga_entity("torus");
	ga_model* torusModel = new ga_model();
	generate_torus(1.5f, 0.7f, 30, torusModel);
	if (dynamic_cast<ga_refractive_lit_material*>(mat))
		computeInteriorDistances(torusModel);

	ga_model_component* sphere_mce = new ga_model_component(torusEnt, torusModel, mat, true);
	sim->add_entity(torusEnt);

	torusEnt->set_position(pos);

	return torusEnt;
}

ga_entity* create_bunny(ga_sim* sim, ga_vec3f pos, ga_material* mat) {
	// procedual sphere
	ga_entity* ent = new ga_entity("bunny");

	if (bunny_model == nullptr) {
		bunny_model = new ga_model();
		assimp_load_model_force("data/models/bunny.obj", bunny_model);
		std::cout << bunny_model->_vertices.size() << std::endl;
	}
	if (dynamic_cast<ga_refractive_lit_material*>(mat)) {
		//std::cout << "Running computeInteriorDistances on the bunny, this may take a while..." << std::endl;
		//computeInteriorDistances(bunny_model);
	}

	ga_model_component* sphere_mce = new ga_model_component(ent, bunny_model, mat, true);
	sim->add_entity(ent);

	ent->set_position(pos);

	return ent;
}

ga_entity* create_light(ga_sim* sim, ga_vec3f pos) {
	// point light ent
	ga_entity* light_entity2 = new ga_entity("point light");
	ga_positional_light* light2 = new ga_positional_light({ 1,1,1 }, 1);
	ga_light_component* light_component2 = new ga_light_component(light_entity2, light2);

	// sphere model of light
	ga_model* lightSphereModel2 = new ga_model();
	generate_sphere(12, lightSphereModel2);
	ga_material* sphereMat2 = new ga_constant_color_material();
	sphereMat2->set_color({ 1,1,1 });
	ga_model_component* sphere_mc3 = new ga_model_component(light_entity2, lightSphereModel2, sphereMat2);
	light_entity2->scale(0.1f);
	sim->add_entity(light_entity2);
	light_entity2->set_position(pos);

	return light_entity2;
}

void process_herald_msg(ga_frame_params* params, ga_sim* sim) {

	TexturePack* selectedTP;

	switch (params->_herald->_selected_mat)
	{
	case(0):
		selectedTP = &my_test_pack;
		break;
	case(1):
		selectedTP = &marble_pack;
		break;
	case(2):
		selectedTP = &rustiron_pack;
		break;
	case(3):
		selectedTP = &steel_pack;
		break;
	case(4):
		selectedTP = &wood_pack;
		break;
	case(5):
		selectedTP = &tile_pack;
		break;
	case(6):
		selectedTP = &moon_pack;
		break;
	default:
		selectedTP = &my_test_pack;
		break;
	}

	ga_material* mat;

	if (params->_herald->_selected_type == 0) {
		mat = new ga_reflective_lit_material(
			selectedTP->texture, 
			selectedTP->normal, 
			sim->get_env_map(), 
			selectedTP->roughness, 
			selectedTP->metallic
		);
		dynamic_cast<ga_reflective_lit_material*>(mat)->set_roughness(selectedTP->roughness_f);
		dynamic_cast<ga_reflective_lit_material*>(mat)->set_metalness(selectedTP->metalness_f);
	}
	else if (params->_herald->_selected_type == 1) {
		mat = new ga_refractive_lit_material(
			selectedTP->texture,
			selectedTP->normal,
			sim->get_env_map(),
			selectedTP->roughness,
			selectedTP->metallic
		);
		dynamic_cast<ga_refractive_lit_material*>(mat)->set_roughness(selectedTP->roughness_f);
		dynamic_cast<ga_refractive_lit_material*>(mat)->set_metalness(selectedTP->metalness_f);
	}



	if (params->_herald->_create_sphere) {
		ga_vec3f pos = params->_camPos;
		ga_vec3f dir = params->_camDir;
		ga_entity* new_ent = create_sphere(sim, pos + dir.scale_result(3.0f*sqrtf(3.0f)), mat);
		sim->select_last_ent();
		params->_herald->_create_sphere = false;
	}
	if (params->_herald->_create_cube) {
		ga_vec3f pos = params->_camPos;
		ga_vec3f dir = params->_camDir;
		ga_entity* new_ent = create_cube(sim, pos + dir.scale_result(3.0f * sqrtf(3.0f)), mat);
		sim->select_last_ent();
		params->_herald->_create_cube = false;
	}
	if (params->_herald->_create_torus) {
		ga_vec3f pos = params->_camPos;
		ga_vec3f dir = params->_camDir;
		ga_entity* new_ent = create_torus(sim, pos + dir.scale_result(3.0f * sqrtf(3.0f)), mat);
		sim->select_last_ent();
		params->_herald->_create_torus = false;
	}
	if (params->_herald->_create_bunny) {
		ga_vec3f pos = params->_camPos;
		ga_vec3f dir = params->_camDir;
		ga_entity* new_ent = create_bunny(sim, pos + dir.scale_result(3.0f * sqrtf(3.0f)), mat);
		sim->select_last_ent();
		params->_herald->_create_bunny = false;
	}
	if (params->_herald->_create_light) {
		ga_vec3f pos = params->_camPos;
		ga_vec3f dir = params->_camDir;
		ga_entity* new_ent = create_light(sim, pos + dir.scale_result(3.0f * sqrtf(3.0f)));
		sim->select_last_ent();
		params->_herald->_create_light = false;
	}
}


