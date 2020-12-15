#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_program.h"
#include "ga_texture.h"
#include "ga_light.h"
#include "ga_geometry.h"

#include "math/ga_mat4f.h"
#include "math/ga_vec3f.h"

#include <string>

/*
** Base class for all graphical materials.
** Includes the shaders and other state necessary to draw geometry.
*/
class ga_material
{
public:
	virtual bool init() = 0;

	virtual bool init(const char* vs_address, const char* fs_address) { return false; }

	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) = 0;

	virtual void set_color(const ga_vec3f& color) {}
};

/*
** Simple unlit material.
*/
class ga_custom_shader_material : public ga_material
{
public:
	ga_custom_shader_material();
	~ga_custom_shader_material();

	virtual bool init() override;
	virtual bool init(const char* vs_address, const char* fs_address) override;
	void SetTexture(const char* texture_path);
	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) override;

private:
	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_texture* _texture;
};

/*
** Simple unlit, single textured material.
*/
class ga_unlit_texture_material : public ga_material
{
public:
	ga_unlit_texture_material(const char* texture_file);
	~ga_unlit_texture_material();

	virtual bool init() override;

	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) override;

private:
	std::string _texture_file;

	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_texture* _texture;
};

/*
** Simple untextured material with a constant color.
*/
class ga_constant_color_material : public ga_material
{
public:
	ga_constant_color_material();
	~ga_constant_color_material();

	virtual bool init() override;

	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) override;

	virtual void set_color(const ga_vec3f& color) override { _color = color; }

private:
	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_vec3f _color;
};

/*
** A material which supports vertex animation.
*/
class ga_animated_material : public ga_material
{
public:
	ga_animated_material(struct ga_skeleton* skeleton);
	~ga_animated_material();

	virtual bool init() override;
	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) override;

private:
	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;

	struct ga_skeleton* _skeleton;
};

/*
** Lit material.
*/
class ga_lit_material : public ga_material
{
public:
	ga_lit_material(const char* texture_file);// , ga_directional_light* light);
	ga_lit_material(const char* texture_file, const char* normalmap_file);
	~ga_lit_material();

	virtual bool init() override;

	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) override;

	virtual void bindLight(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform, const struct ga_light_drawcall& lights, const ga_mat4f& shadowMVP);

private:
	std::string _texture_file;
	std::string _normalmap_file;

	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_texture* _texture;
	ga_texture* _normalmap;
	ga_vec3f _baseColor;
	ga_vec3f _ambientLight;

	//class ga_directional_light* _light;
};

// material with texture & light
class ga_lit_anim_material : public ga_material {
public :
	ga_lit_anim_material(struct ga_skeleton* skeleton, class ga_directional_light* light);
	~ga_lit_anim_material();

	virtual bool init() override;

	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) override;

private:
	std::string _texture_file;

	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_vec3f _baseColor;
	ga_vec3f _ambientLight;

	class ga_directional_light* _light;

	struct ga_skeleton* _skeleton;
};

// material with texture & light
class ga_tess_plane_material : public ga_material
{
public:
	ga_tess_plane_material(const char* texture_file, const char* heightmap_file);// , ga_directional_light* light
	~ga_tess_plane_material();

	virtual bool init() override;

	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) override;

	void bindPatch(ga_patch* patch);

private:
	std::string _texture_file;
	std::string _height_file;

	ga_shader* _vs;
	ga_shader* _fs;
	ga_shader* _tcs;
	ga_shader* _tes;
	ga_program* _program;
	ga_texture* _texture;
	ga_texture* _heightmap;
	ga_patch* _patch;
};


class ga_terrain_material : public ga_material
{
public:
	ga_terrain_material(const char* texture_file, const char* heightmap_file, const char* normal_file);// , ga_directional_light* light
	~ga_terrain_material();

	virtual bool init() override;

	virtual void bind(const ga_mat4f& view_proj, const ga_mat4f& transform) override;

	virtual void bindLight(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform, const struct ga_light_drawcall& lights, const ga_mat4f& shadowMVP);

	void bindTerrain(ga_terrain* terrain);

private:
	std::string _texture_file;
	std::string _height_file;
	std::string _normal_file;

	ga_shader* _vs;
	ga_shader* _fs;
	ga_shader* _tcs;
	ga_shader* _tes;
	ga_program* _program;
	ga_texture* _texture;
	ga_texture* _heightmap;
	ga_texture* _normalmap;
	ga_vec3f _baseColor;
	ga_vec3f _ambientLight;
	ga_terrain* _terrain;
};