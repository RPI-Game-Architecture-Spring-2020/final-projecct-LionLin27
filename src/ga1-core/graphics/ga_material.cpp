/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_material.h"
#include "ga_animation.h"
#include "ga_light.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

void load_shader(const char* filename, std::string& contents)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::ifstream file(fullpath);

	assert(file.is_open());

	contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

ga_unlit_texture_material::ga_unlit_texture_material(const char* texture_file) :
	_texture_file(texture_file)
{
}

ga_unlit_texture_material::~ga_unlit_texture_material()
{
}

bool ga_unlit_texture_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_unlit_texture_vert.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/ga_unlit_texture_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);
	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	_texture = new ga_texture();
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load texture" << std::endl;
	}

	return true;
}

void ga_unlit_texture_material::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");

	_program->use();

	mvp_uniform.set(transform * view_proj);
	texture_uniform.set(*_texture, 0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

ga_custom_shader_material::ga_custom_shader_material()
{
}

ga_custom_shader_material::~ga_custom_shader_material()
{
}

bool ga_custom_shader_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_unlit_vert.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/ga_unlit_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);
	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	return true;
}

bool ga_custom_shader_material::init(const char* vs_address, const char* fs_address)
{
	// default vs: data/shaders/ga_unlit_vert.glsl
	// default fs: data/shaders/ga_unlit_frag.glsl
	std::string source_vs;
	load_shader(vs_address, source_vs);

	std::string source_fs;
	load_shader(fs_address, source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);
	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	return true;
}

void ga_custom_shader_material::SetTexture(const char* texture_path)
{
	_texture = new ga_texture();
	if (!_texture->load_from_file(texture_path))
	{
		std::cerr << "Failed to load rpi.png" << std::endl;
	}
}

void ga_custom_shader_material::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("texCoord");
	_program->use();
	

	mvp_uniform.set(transform * view_proj);
	texture_uniform.set(*_texture, 0);

	//glActiveTexture(GL_TEXTURE0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

ga_constant_color_material::ga_constant_color_material()
{
}

ga_constant_color_material::~ga_constant_color_material()
{
}

bool ga_constant_color_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_constant_color_vert.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/ga_constant_color_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);
	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	return true;
}

void ga_constant_color_material::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform color_uniform = _program->get_uniform("u_color");

	_program->use();

	mvp_uniform.set(transform * view_proj);
	color_uniform.set(_color);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

ga_animated_material::ga_animated_material(ga_skeleton* skeleton) : _skeleton(skeleton)
{
}

ga_animated_material::~ga_animated_material()
{
}

bool ga_animated_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_animated_vert_hw6.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/ga_animated_frag_hw6.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);
	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	return true;
}

void ga_animated_material::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform skin_uniform = _program->get_uniform("u_skin");

	_program->use();

	mvp_uniform.set(transform * view_proj);
	
	// Collect the skinning matrices.
	ga_mat4f skin[ga_skeleton::k_max_skeleton_joints];
	for (uint32_t i = 0; i < _skeleton->_joints.size(); ++i)
	{
		assert(i < ga_skeleton::k_max_skeleton_joints);
		skin[i] = _skeleton->_joints[i]->_skin;
	}
	skin_uniform.set(skin, ga_skeleton::k_max_skeleton_joints);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

ga_lit_anim_material::ga_lit_anim_material(ga_skeleton* skeleton, ga_directional_light* light) : _skeleton(skeleton), _light(light)
{
}

ga_lit_anim_material::~ga_lit_anim_material()
{
}

bool ga_lit_anim_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_animated_vert_hw6.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/ga_animated_frag_hw6.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);
	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}


	_baseColor = { 1.0, 1.0, 1.0 };

	return true;
}

void ga_lit_anim_material::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	_ambientLight = { 0.1, 0.1, 0.1 };

	_program->use();

	//ga_vec3f light_color = { 0.9, 0.6, 0.1 };
	ga_vec3f light_color = _light->_color;
	float intensity = _light->_intensity;
	ga_vec3f direction = _light->_direction;

	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform trans_uniform = _program->get_uniform("u_trans");
	ga_uniform skin_uniform = _program->get_uniform("u_skin");

	ga_uniform baseColor_uniform = _program->get_uniform("u_baseColor");
	ga_uniform ambient_uniform = _program->get_uniform("u_ambientLight");

	ga_uniform light_color_uniform = _program->get_uniform("u_directionalLight.base.color");
	ga_uniform light_intensity_uniform = _program->get_uniform("u_directionalLight.base.intensity");
	ga_uniform light_direction_uniform = _program->get_uniform("u_directionalLight.direction");

	mvp_uniform.set(transform * view_proj);
	trans_uniform.set(transform);
	// Collect the skinning matrices.
	ga_mat4f skin[ga_skeleton::k_max_skeleton_joints];
	for (uint32_t i = 0; i < _skeleton->_joints.size(); ++i)
	{
		assert(i < ga_skeleton::k_max_skeleton_joints);
		skin[i] = _skeleton->_joints[i]->_skin;
	}
	skin_uniform.set(skin, ga_skeleton::k_max_skeleton_joints);

	baseColor_uniform.set(_baseColor);
	ambient_uniform.set(_ambientLight);

	ga_directional_light light(light_color, intensity);

	light_color_uniform.set(light_color);
	light_intensity_uniform.set(intensity);
	light_direction_uniform.set(light._direction);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

ga_lit_material::ga_lit_material(const char* texture_file) :
	_texture_file(texture_file)
{
}

ga_lit_material::ga_lit_material(const char* texture_file, const char* normalmap_file) :
	_texture_file(texture_file), _normalmap_file(normalmap_file)
{
}

ga_lit_material::~ga_lit_material()
{
}

bool ga_lit_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_lit_vert.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/ga_lit_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);

	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}


	_texture = new ga_texture();
	_useTextureMap = true;
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load texture" << std::endl;
		_useTextureMap = false;
	}


	_useNormalMap = false;
	if (_normalmap_file.length() > 0) {
		_useNormalMap = true;
		_normalmap = new ga_texture();
		if (!_normalmap->load_from_file(_normalmap_file.c_str()))
		{
			std::cerr << "Failed to load normal map" << std::endl;
			_useNormalMap = false;
		}
	}

	_baseColor = { 1,1,1 };

	return true;
}

void ga_lit_material::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	return;
	/*
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");

	_program->use();

	mvp_uniform.set(transform * view_proj);
	texture_uniform.set(*_texture, 0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	*/
}



void ga_lit_material::bindLight(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform, const struct ga_light_drawcall& lights, const ga_mat4f& shadowMVP)
{
	_ambientLight = { 0.1, 0.1, 0.1 };

	_program->use();

	// directional light
	ga_directional_light* dirL = lights._dirLight;

	// get the locations of the light and material fields in the shader
	ga_uniform globalAmbLoc = _program->get_uniform("u_ambientLight");
	// ga_uniform ambLoc = _program->get_uniform("u_directionalLight.base.ambient");
	ga_uniform diffLoc = _program->get_uniform("u_directionalLight.base.color");
	// ga_uniform specLoc = _program->get_uniform("light.specular");
	ga_uniform dirLoc = _program->get_uniform("u_directionalLight.direction");
	ga_uniform itsLoc = _program->get_uniform("u_directionalLight.base.intensity");
	// ga_uniform mambLoc = _program->get_uniform("material.ambient");
	// ga_uniform mdiffLoc = _program->get_uniform("material.diffuse");
	// ga_uniform mspecLoc = _program->get_uniform("material.specular");
	// ga_uniform mshiLoc = _program->get_uniform("material.shininess");
	ga_uniform shadowMVPLoc = _program->get_uniform("shadowMVP");

	//  set the uniform light and material values in the shader
	ga_uniform baseColor_uniform = _program->get_uniform("u_baseColor");
	baseColor_uniform.set(_baseColor);

	globalAmbLoc.set(_ambientLight);
	// ambLoc.set(lightAmbient);
	diffLoc.set(dirL->_color);
	// specLoc.set(lightSpecular);
	dirLoc.set(view.transform_vector(dirL->_direction));
	itsLoc.set(dirL->_intensity);
	// mambLoc.set(matAmb);
	// mdiffLoc.set(matDif);
	// mspecLoc.set(matSpe);
	// mshiLoc.set(matShi);
	shadowMVPLoc.set(shadowMVP);

	// positional lights
	ga_uniform posLightCountLoc = _program->get_uniform("u_posLightCount");
	float posLCount = 0.1;
	for (ga_positional_light* posL : lights._posLightArr) {
		ga_vec3f transformedPos = view.transform_point(posL->_position);
		std::string index = std::to_string((int)posLCount);
		std::string diffName = "u_positionalLights[" + index + "].base.color";
		std::string posName = "u_positionalLights[" + index + "].position";
		std::string itsName = "u_positionalLights[" + index + "].base.intensity";
		ga_uniform diffLoc_p = _program->get_uniform(diffName.c_str());
		ga_uniform posLoc_p = _program->get_uniform(posName.c_str());
		ga_uniform itsLoc_p = _program->get_uniform(itsName.c_str());

		diffLoc_p.set(posL->_color);
		posLoc_p.set(transformedPos);
		itsLoc_p.set(posL->_intensity);

		posLCount += 1.0;
	}
	posLightCountLoc.set(posLCount);


	ga_uniform mvMat = _program->get_uniform("u_mvMat");
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");
	mvMat.set(transform * view);
	mvp_uniform.set(transform * view * proj);
	texture_uniform.set(*_texture, 0);

	ga_uniform useTexture = _program->get_uniform("b_useTexture");
	useTexture.set(true);

	ga_uniform useNormalMap = _program->get_uniform("b_useNormalMap");
	ga_uniform normalmap_uniform = _program->get_uniform("u_normMap");
	
	useNormalMap.set(_useNormalMap);
	if (_useNormalMap) {
		normalmap_uniform.set(*_normalmap, 2);
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void ga_lit_material::bind_texture(std::string file_name)
{
	_texture_file = file_name;
	_texture = new ga_texture();
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load texture" << std::endl;
		return;
	}

	_program->use();
	ga_uniform texture_uniform = _program->get_uniform("u_texture");
	texture_uniform.set(*_texture, 0);
}

void ga_lit_material::bind_normalMap(std::string file_name)
{
	_normalmap_file = file_name;
	_normalmap = new ga_texture();
	if (!_normalmap->load_from_file(_normalmap_file.c_str()))
	{
		std::cerr << "Failed to load normal map" << std::endl;
		return;
	}

	_program->use();
	ga_uniform normalmap_uniform = _program->get_uniform("u_normMap");
	normalmap_uniform.set(*_normalmap, 2);
}

void ga_lit_material::set_useNormalMap(bool use)
{
	_useNormalMap = use;

	_program->use();
	ga_uniform useNormalMap = _program->get_uniform("b_useNormalMap");
	useNormalMap.set(_useNormalMap);
}

void ga_lit_material::set_baseColor(ga_vec3f color)
{
	_program->use();
	ga_uniform baseColorUniform = _program->get_uniform("u_baseColor");
	baseColorUniform.set(color);
	_baseColor = color;
}

void ga_lit_material::set_useTextureMap(bool use)
{
	_useTextureMap = use;

	_program->use();
	ga_uniform useTextureMap = _program->get_uniform("b_useTexture");
	useTextureMap.set(_useTextureMap);
}

// tesselation shader for plane
ga_tess_plane_material::ga_tess_plane_material(const char* texture_file, const char* heightmap_file) :
	_texture_file(texture_file), _height_file(heightmap_file)
{
}

ga_tess_plane_material::~ga_tess_plane_material()
{
}

bool ga_tess_plane_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_tvs.glsl", source_vs);

	std::string source_tcs;
	load_shader("data/shaders/ga_tcs.glsl", source_tcs);

	std::string source_tes;
	load_shader("data/shaders/ga_tes.glsl", source_tes);

	std::string source_fs;
	load_shader("data/shaders/ga_tfs.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;

	_tcs = new ga_shader(source_tcs.c_str(), GL_TESS_CONTROL_SHADER);
	if (!_tcs->compile())
		std::cerr << "Failed to compile tess control shader:" << std::endl << _tcs->get_compile_log() << std::endl;

	_tes = new ga_shader(source_tes.c_str(), GL_TESS_EVALUATION_SHADER);
	if (!_tes->compile())
		std::cerr << "Failed to compile tess evaluation shader:" << std::endl << _tes->get_compile_log() << std::endl;

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_tcs);
	_program->attach(*_tes);
	_program->attach(*_fs);

	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	_texture = new ga_texture();
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load texture" << std::endl;
	}

	_heightmap = new ga_texture();
	if (!_heightmap->load_from_file(_height_file.c_str()))
	{
		std::cerr << "Failed to load height map" << std::endl;
	}

	return true;
}

void ga_tess_plane_material::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	_program->use();

	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");
	ga_uniform height_uniform = _program->get_uniform("u_height");
	ga_uniform controls_uniform = _program->get_uniform("u_controls");
	ga_uniform tesslvl_uniform = _program->get_uniform("TL");

	mvp_uniform.set(transform * view_proj);
	texture_uniform.set(*_texture, 0);
	height_uniform.set(*_heightmap, 2);
	controls_uniform.set(_patch->_controls);
	tesslvl_uniform.set(_patch->_tess_lvl);

	glPatchParameteri(GL_PATCH_VERTICES, 16);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);

	return;
}

void ga_tess_plane_material::bindPatch(ga_patch* patch)
{
	_patch = patch;
}



// tesselation terrain
ga_terrain_material::ga_terrain_material(const char* texture_file, const char* heightmap_file, const char* normal_file) :
	_texture_file(texture_file), _height_file(heightmap_file), _normal_file(normal_file)
{
}

ga_terrain_material::~ga_terrain_material()
{
}

void ga_terrain_material::bindTerrain(ga_terrain* terrain)
{
	_terrain = terrain;
}

bool ga_terrain_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_terrain_tvs.glsl", source_vs);

	std::string source_tcs;
	load_shader("data/shaders/ga_terrain_tcs.glsl", source_tcs);

	std::string source_tes;
	load_shader("data/shaders/ga_terrain_tes.glsl", source_tes);

	std::string source_fs;
	load_shader("data/shaders/ga_terrain_tfs.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;

	_tcs = new ga_shader(source_tcs.c_str(), GL_TESS_CONTROL_SHADER);
	if (!_tcs->compile())
		std::cerr << "Failed to compile tess control shader:" << std::endl << _tcs->get_compile_log() << std::endl;

	_tes = new ga_shader(source_tes.c_str(), GL_TESS_EVALUATION_SHADER);
	if (!_tes->compile())
		std::cerr << "Failed to compile tess evaluation shader:" << std::endl << _tes->get_compile_log() << std::endl;

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_tcs);
	_program->attach(*_tes);
	_program->attach(*_fs);

	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	_texture = new ga_texture();
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load texture" << std::endl;
	}

	_heightmap = new ga_texture();
	if (!_heightmap->load_from_file(_height_file.c_str()))
	{
		std::cerr << "Failed to load height map" << std::endl;
	}

	_normalmap = new ga_texture();
	if (!_normalmap->load_from_file(_normal_file.c_str()))
	{
		std::cerr << "Failed to load normal map" << std::endl;
	}

	return true;
}

void ga_terrain_material::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	_program->use();

	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");
	ga_uniform height_uniform = _program->get_uniform("u_height");
	ga_uniform subdiv_uniform = _program->get_uniform("u_subdivisions");

	mvp_uniform.set(transform * view_proj);
	texture_uniform.set(*_texture, 0);
	height_uniform.set(*_heightmap, 2);
	subdiv_uniform.set(_terrain->_subdivision);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);

	return;
}

void ga_terrain_material::bindLight(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform, const struct ga_light_drawcall& lights, const ga_mat4f& shadowMVP)
{
	_ambientLight = { 0.1, 0.1, 0.1 };

	_program->use();

	// directional light
	ga_directional_light* dirL = lights._dirLight;

	// get the locations of the light and material fields in the shader
	ga_uniform globalAmbLoc = _program->get_uniform("u_ambientLight");
	ga_uniform diffLoc = _program->get_uniform("u_directionalLight.base.color");
	ga_uniform dirLoc = _program->get_uniform("u_directionalLight.direction");
	ga_uniform itsLoc = _program->get_uniform("u_directionalLight.base.intensity");
	//ga_uniform shadowMVPLoc = _program->get_uniform("shadowMVP");

	//  set the uniform light and material values in the shader
	globalAmbLoc.set(_ambientLight);
	// ambLoc.set(lightAmbient);
	diffLoc.set(dirL->_color);
	// specLoc.set(lightSpecular);
	dirLoc.set(view.transform_vector(dirL->_direction));
	itsLoc.set(dirL->_intensity);
	// mambLoc.set(matAmb);
	// mdiffLoc.set(matDif);
	// mspecLoc.set(matSpe);
	// mshiLoc.set(matShi);
	//shadowMVPLoc.set(shadowMVP);

	// positional lights
	ga_uniform posLightCountLoc = _program->get_uniform("u_posLightCount");
	float posLCount = 0.1;
	for (ga_positional_light* posL : lights._posLightArr) {
		ga_vec3f transformedPos = view.transform_point(posL->_position);
		std::string index = std::to_string((int)posLCount);
		std::string diffName = "u_positionalLights[" + index + "].base.color";
		std::string posName = "u_positionalLights[" + index + "].position";
		std::string itsName = "u_positionalLights[" + index + "].base.intensity";
		ga_uniform diffLoc_p = _program->get_uniform(diffName.c_str());
		ga_uniform posLoc_p = _program->get_uniform(posName.c_str());
		ga_uniform itsLoc_p = _program->get_uniform(itsName.c_str());

		diffLoc_p.set(posL->_color);
		posLoc_p.set(transformedPos);
		itsLoc_p.set(posL->_intensity);

		posLCount += 1.0;
	}
	posLightCountLoc.set(posLCount);


	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");
	ga_uniform height_uniform = _program->get_uniform("u_height");
	ga_uniform normalmap_uniform = _program->get_uniform("u_normMap");
	ga_uniform mvMat = _program->get_uniform("u_mvMat");
	ga_uniform subdiv_uniform = _program->get_uniform("u_subdivisions");
	
	mvp_uniform.set(transform * view * proj);
	texture_uniform.set(*_texture, 0);
	height_uniform.set(*_heightmap, 2);
	normalmap_uniform.set(*_normalmap, 3);
	mvMat.set(transform * view);
	subdiv_uniform.set(_terrain->_subdivision);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);

	return;
}

ga_reflective_lit_material::ga_reflective_lit_material(const char* texture_file, const char* normalmap_file, ga_cube_texture* env_map, const char* roughMap_file, const char* metalMap_file, const char* aoMap_file) :
	ga_lit_material(texture_file, normalmap_file), _envMap(env_map), _metallic_file(metalMap_file), _roughness_file(roughMap_file), _ao_file(aoMap_file)
{
	_useEnvMap = true;
	_roughness = 0.1f;
	_metalness = 0.5f;
	_rcvShadow = false;
}

bool ga_reflective_lit_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/pbr_vert.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/pbr_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);

	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}


	_texture = new ga_texture();
	_useTextureMap = true;
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load textures" << std::endl;
		_useTextureMap = false;
	}


	_useNormalMap = false;
	if (_normalmap_file.length() > 0) {
		_useNormalMap = true;
		_normalmap = new ga_texture();
		if (!_normalmap->load_from_file(_normalmap_file.c_str()))
		{
			std::cerr << "Failed to load normal map" << std::endl;
			_useNormalMap = false;
		}
	}

	_useRoughMap = false;
	if (_roughness_file.length() > 0) {
		_useRoughMap = true;
		_roughnessMap = new ga_texture();
		if (!_roughnessMap->load_from_file(_roughness_file.c_str()))
		{
			std::cerr << "Failed to load roughness map" << std::endl;
			_useRoughMap = false;
		}
	}

	_useMetalMap = false;
	if (_metallic_file.length() > 0) {
		_useMetalMap = true;
		_metallicMap = new ga_texture();
		if (!_metallicMap->load_from_file(_metallic_file.c_str()))
		{
			std::cerr << "Failed to load metallic map" << std::endl;
			_useMetalMap = false;
		}
	}

	_useAOMap = false;
	if (_ao_file.length() > 0) {
		_useAOMap = true;
		_aoMap = new ga_texture();
		if (!_aoMap->load_from_file(_ao_file.c_str())) {
			std::cerr << "Failed to load ao map" << std::endl;
			_useAOMap = false;
		}
	}


	// set initial roughness to 0
	//_roughness = 0.1f;
	//_metalness = 0.5f;
	_normalStr = 1.0f;

	_NDF = true;
	_GEO = true;
	_FNL = true;
	_debug_uniform = 0;
	_NDF_selection = 1;
	_GEO_selection = 1;
	_FNL_selection = 1;

	_baseColor = { 1,1,1 };

	return true;
}


void ga_reflective_lit_material::bindLight(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform, const struct ga_light_drawcall& lights, const ga_mat4f& shadowMVP)
{
	_ambientLight = { 0.3, 0.3, 0.3 };

	_program->use();

	// directional light
	ga_directional_light* dirL = lights._dirLight;

	// temprory shadow hack
	if (_rcvShadow) {
		_ambientLight = { 0.1, 0.1, 0.1 };
	}
	else {
		_ambientLight = { 0.3, 0.3, 0.3 };
	}

	// get the locations of the light and material fields in the shader
	ga_uniform globalAmbLoc = _program->get_uniform("u_ambientLight");
	ga_uniform diffLoc = _program->get_uniform("u_directionalLight.base.color");
	ga_uniform dirLoc = _program->get_uniform("u_directionalLight.direction");
	ga_uniform itsLoc = _program->get_uniform("u_directionalLight.base.intensity");
	ga_uniform shadowMVPLoc = _program->get_uniform("shadowMVP");

	//  set the uniform light and material values in the shader
	globalAmbLoc.set(_ambientLight);
	diffLoc.set(dirL->_color);
	dirLoc.set(view.transform_vector(dirL->_direction));
	itsLoc.set(dirL->_intensity);
	shadowMVPLoc.set(shadowMVP);

	// positional lights
	ga_uniform posLightCountLoc = _program->get_uniform("u_posLightCount");
	float posLCount = 0.1;
	for (ga_positional_light* posL : lights._posLightArr) {
		ga_vec3f transformedPos = view.transform_point(posL->_position);
		std::string index = std::to_string((int)posLCount);
		std::string diffName = "u_positionalLights[" + index + "].base.color";
		std::string posName = "u_positionalLights[" + index + "].position";
		std::string itsName = "u_positionalLights[" + index + "].base.intensity";
		ga_uniform diffLoc_p = _program->get_uniform(diffName.c_str());
		ga_uniform posLoc_p = _program->get_uniform(posName.c_str());
		ga_uniform itsLoc_p = _program->get_uniform(itsName.c_str());

		diffLoc_p.set(posL->_color);
		posLoc_p.set(transformedPos);
		itsLoc_p.set(posL->_intensity);

		posLCount += 1.0;
	}
	posLightCountLoc.set(posLCount);

	ga_uniform baseColor_uniform = _program->get_uniform("u_baseColor");
	baseColor_uniform.set(_baseColor);

	// env map
	ga_uniform envMap_uniform = _program->get_uniform("u_envMap");
	envMap_uniform.set(*_envMap, 5);// no particular reason for 5..

	// fix reflection
	ga_uniform vMat = _program->get_uniform("u_vMat");
	vMat.set(view);


	ga_uniform mvMat = _program->get_uniform("u_mvMat");
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");
	mvMat.set(transform * view);
	mvp_uniform.set(transform * view * proj);
	texture_uniform.set(*_texture, 0);

	ga_uniform useTexture = _program->get_uniform("b_useTexture");
	useTexture.set(_useTextureMap);

	ga_uniform useNormalMap = _program->get_uniform("b_useNormalMap");
	ga_uniform normalmap_uniform = _program->get_uniform("u_normMap");

	useNormalMap.set(_useNormalMap);
	if (_useNormalMap) {
		normalmap_uniform.set(*_normalmap, 1);
	}
	
	// roughness
	ga_uniform roughness_uniform = _program->get_uniform("f_roughness");
	roughness_uniform.set(_roughness);

	ga_uniform metalness_uniform = _program->get_uniform("f_metalness");
	metalness_uniform.set(_metalness);

	ga_uniform normalStr_uniform = _program->get_uniform("f_normalStr");
	normalStr_uniform.set(_normalStr);

	ga_uniform useRoughMap = _program->get_uniform("b_useRoughMap");
	ga_uniform roughmap_uniform = _program->get_uniform("u_roughnessMap");
	useRoughMap.set(_useRoughMap);
	if (_useRoughMap) {
		roughmap_uniform.set(*_roughnessMap, 2);
	}

	ga_uniform useMetalMap = _program->get_uniform("b_useMetalMap");
	ga_uniform metalmap_uniform = _program->get_uniform("u_metallicMap");
	useMetalMap.set(_useMetalMap);
	if (_useMetalMap) {
		metalmap_uniform.set(*_metallicMap, 3);
	}

	ga_uniform useAOMap = _program->get_uniform("b_useAOMap");
	ga_uniform aomap_uniform = _program->get_uniform("u_AOMap");
	useAOMap.set(_useAOMap);
	if (_useAOMap) {
		aomap_uniform.set(*_aoMap, 4);
	}

	ga_uniform useEnvMap = _program->get_uniform("b_useEnvMap");
	useEnvMap.set(_useEnvMap);

	ga_uniform rcvShadow = _program->get_uniform("b_rcvShadow");
	rcvShadow.set(_rcvShadow);

	// for visualization
	ga_uniform ndf = _program->get_uniform("b_NDF");
	ga_uniform geo = _program->get_uniform("b_GEO");
	ga_uniform fnl = _program->get_uniform("b_FNL");
	ndf.set(_NDF);
	geo.set(_GEO);
	fnl.set(_FNL);

	ga_uniform ndf_selection = _program->get_uniform("u_NDFSelection");
	ga_uniform geo_selection = _program->get_uniform("u_GEOSelection");
	ga_uniform fnl_selection = _program->get_uniform("u_FNLSelection");
	ndf_selection.set(_NDF_selection);
	geo_selection.set(_GEO_selection);
	fnl_selection.set(_FNL_selection);

	ga_uniform debug_u = _program->get_uniform("u_debug");
	debug_u.set(_debug_uniform);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void ga_reflective_lit_material::set_roughness(float roughness) {
	_roughness = roughness;

	//_program->use();
	//ga_uniform roughnessUniform = _program->get_uniform("f_roughness");
	//roughnessUniform.set(roughness);
}
void ga_reflective_lit_material::switch_brdf_comp(bool _ndf, bool _geo, bool _fnl)
{
	_NDF = _ndf;
	_GEO = _geo;
	_FNL = _fnl;

	_program->use();

	ga_uniform ndf = _program->get_uniform("b_NDF");
	ga_uniform geo = _program->get_uniform("b_GEO");
	ga_uniform fnl = _program->get_uniform("b_FNL");
	ndf.set(_NDF);
	geo.set(_GEO);
	fnl.set(_FNL);
}
float ga_reflective_lit_material::get_roughness() {
	return _roughness;
}


// REFRACTIVE LIT MATERIAL
ga_refractive_lit_material::ga_refractive_lit_material(const char* texture_file, const char* normalmap_file, ga_cube_texture* env_map, const char* roughMap_file, const char* metalMap_file) :
	ga_lit_material(texture_file, normalmap_file), _envMap(env_map), _metallic_file(metalMap_file), _roughness_file(roughMap_file)
{
	_useEnvMap = true;
	_roughness = 0.1f;
	_metalness = 0.5f;
	_clearity = 1.0f;
}

bool ga_refractive_lit_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/refraction_vert.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/refraction_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);

	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}


	_texture = new ga_texture();
	_useTextureMap = true;
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load texture" << std::endl;
		_useTextureMap = false;
	}


	_useNormalMap = false;
	if (_normalmap_file.length() > 0) {
		_useNormalMap = true;
		_normalmap = new ga_texture();
		if (!_normalmap->load_from_file(_normalmap_file.c_str()))
		{
			std::cerr << "Failed to load normal map" << std::endl;
			_useNormalMap = false;
		}
	}

	_useRoughMap = false;
	if (_roughness_file.length() > 0) {
		_useRoughMap = true;
		_roughnessMap = new ga_texture();
		if (!_roughnessMap->load_from_file(_roughness_file.c_str()))
		{
			std::cerr << "Failed to load roughness map" << std::endl;
			_useRoughMap = false;
		}
	}

	_useMetalMap = false;
	if (_metallic_file.length() > 0) {
		_useMetalMap = true;
		_metallicMap = new ga_texture();
		if (!_metallicMap->load_from_file(_metallic_file.c_str()))
		{
			std::cerr << "Failed to load metallic map" << std::endl;
			_useMetalMap = false;
		}
	}


	// set initial roughness to 0
	//_roughness = 0.1f;
	//_metalness = 0.5f;
	_normalStr = 1.0f;
	_ior = 1.0f;

	_baseColor = { 1.0, 1.0, 1.0 };

	return true;
}


void ga_refractive_lit_material::bindLight(const ga_mat4f& view, const ga_mat4f& proj,
	const ga_mat4f& transform, const struct ga_light_drawcall& lights, const ga_mat4f& shadowMVP,
	int depthTexIndex, int normalTexIndex, const ga_mat4f& u_eyeProj, const ga_vec3f& eyePos,
	const ga_mat4f& vpMat_for_world_proj)
{
	_ambientLight = { 0.3, 0.3, 0.3 };

	_program->use();

	// directional light
	ga_directional_light* dirL = lights._dirLight;

	// get the locations of the light and material fields in the shader
	ga_uniform globalAmbLoc = _program->get_uniform("u_ambientLight");
	// ga_uniform ambLoc = _program->get_uniform("u_directionalLight.base.ambient");
	ga_uniform diffLoc = _program->get_uniform("u_directionalLight.base.color");
	// ga_uniform specLoc = _program->get_uniform("light.specular");
	ga_uniform dirLoc = _program->get_uniform("u_directionalLight.direction");
	ga_uniform itsLoc = _program->get_uniform("u_directionalLight.base.intensity");
	// ga_uniform mambLoc = _program->get_uniform("material.ambient");
	// ga_uniform mdiffLoc = _program->get_uniform("material.diffuse");
	// ga_uniform mspecLoc = _program->get_uniform("material.specular");
	// ga_uniform mshiLoc = _program->get_uniform("material.shininess");
	ga_uniform shadowMVPLoc = _program->get_uniform("shadowMVP");

	//  set the uniform light and material values in the shader
	ga_uniform baseColor_uniform = _program->get_uniform("u_baseColor");
	baseColor_uniform.set(_baseColor);

	globalAmbLoc.set(_ambientLight);
	// ambLoc.set(lightAmbient);
	diffLoc.set(dirL->_color);
	// specLoc.set(lightSpecular);
	dirLoc.set(view.transform_vector(dirL->_direction));
	itsLoc.set(dirL->_intensity);
	// mambLoc.set(matAmb);
	// mdiffLoc.set(matDif);
	// mspecLoc.set(matSpe);
	// mshiLoc.set(matShi);
	shadowMVPLoc.set(shadowMVP);

	// positional lights
	ga_uniform posLightCountLoc = _program->get_uniform("u_posLightCount");
	float posLCount = 0.1;
	for (ga_positional_light* posL : lights._posLightArr) {
		ga_vec3f transformedPos = view.transform_point(posL->_position);
		std::string index = std::to_string((int)posLCount);
		std::string diffName = "u_positionalLights[" + index + "].base.color";
		std::string posName = "u_positionalLights[" + index + "].position";
		std::string itsName = "u_positionalLights[" + index + "].base.intensity";
		ga_uniform diffLoc_p = _program->get_uniform(diffName.c_str());
		ga_uniform posLoc_p = _program->get_uniform(posName.c_str());
		ga_uniform itsLoc_p = _program->get_uniform(itsName.c_str());

		diffLoc_p.set(posL->_color);
		posLoc_p.set(transformedPos);
		itsLoc_p.set(posL->_intensity);

		posLCount += 1.0;
	}
	posLightCountLoc.set(posLCount);

	// env map
	ga_uniform envMap_uniform = _program->get_uniform("u_envMap");
	envMap_uniform.set(*_envMap, 5);// no particular reason for 5..

	// fix reflection
	ga_uniform vMat = _program->get_uniform("u_vMat");
	vMat.set(view);

	// clearity
	ga_uniform clearity = _program->get_uniform("f_clearity");
	clearity.set(_clearity);

	ga_uniform eyePos_uniform = _program->get_uniform("v_eyePos");
	eyePos_uniform.set(eyePos);

	ga_uniform mvMat = _program->get_uniform("u_mvMat");
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");
	mvMat.set(transform * view);
	mvp_uniform.set(transform * view * proj);
	texture_uniform.set(*_texture, 0);

	ga_uniform vpMat_uniform = _program->get_uniform("u_vpMat_proj");
	vpMat_uniform.set(vpMat_for_world_proj);

	ga_uniform eyeProj_uniform = _program->get_uniform("u_eyeProj");
	eyeProj_uniform.set(u_eyeProj);

	ga_uniform useTexture = _program->get_uniform("b_useTexture");
	useTexture.set(_useTextureMap);

	ga_uniform useNormalMap = _program->get_uniform("b_useNormalMap");
	ga_uniform normalmap_uniform = _program->get_uniform("u_normMap");
	useNormalMap.set(_useNormalMap);
	if (_useNormalMap) {
		normalmap_uniform.set(*_normalmap, 1);
	}

	ga_uniform useRoughMap = _program->get_uniform("b_useRoughMap");
	ga_uniform roughmap_uniform = _program->get_uniform("u_roughnessMap");
	useRoughMap.set(_useRoughMap);
	if (_useRoughMap) {
		roughmap_uniform.set(*_roughnessMap, 2);
	}

	ga_uniform useMetalMap = _program->get_uniform("b_useMetalMap");
	ga_uniform metalmap_uniform = _program->get_uniform("u_metallicMap");
	useMetalMap.set(_useMetalMap);
	if (_useMetalMap) {
		metalmap_uniform.set(*_metallicMap, 3);
	}

	// roughness
	ga_uniform roughness_uniform = _program->get_uniform("f_roughness");
	roughness_uniform.set(_roughness);

	ga_uniform metalness_uniform = _program->get_uniform("f_metalness");
	metalness_uniform.set(_metalness);

	ga_uniform normalStr_uniform = _program->get_uniform("f_normalStr");
	normalStr_uniform.set(_normalStr);
	ga_uniform ior_uniform = _program->get_uniform("f_ior");
	ior_uniform.set(_ior);



	ga_uniform backNormal_uniform = _program->get_uniform("u_backNormals");
	ga_uniform backDepth_uniform = _program->get_uniform("u_backDepths");

	// Properties and normal/depth buffers are Hardcoded to 10 and 11

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}