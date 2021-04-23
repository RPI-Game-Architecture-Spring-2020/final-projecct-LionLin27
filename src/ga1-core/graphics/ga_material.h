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

	virtual const char* get_name() { return "nameless material"; }
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

	virtual ga_vec3f get_color() { return _color; }
	virtual void set_color(const ga_vec3f& color) override { _color = color; }

	virtual const char* get_name() override { return "constant color material"; }

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

	// for ui display and dynamic change
	virtual const char* get_name() override { return "lit material"; }
	std::string get_texture_file() { return _texture_file; }
	void bind_texture(std::string file_name);
	bool get_useTexturelMap() { return _useTextureMap; };
	void set_useTextureMap(bool use);
	void bind_normalMap(std::string file_name);
	bool get_useNormalMap() { return _useNormalMap; };
	void set_useNormalMap(bool use);
	ga_vec3f get_baseColor() { return _baseColor; }
	void set_baseColor(ga_vec3f color);


protected:
	std::string _texture_file;
	std::string _normalmap_file;

	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_texture* _texture;
	ga_texture* _normalmap;
	ga_vec3f _baseColor;
	ga_vec3f _ambientLight;

	bool _useNormalMap;
	bool _useTextureMap;

	//class ga_directional_light* _light;
};

class ga_reflective_lit_material : public ga_lit_material {
public:
	ga_reflective_lit_material(const char* texture_file, const char* normalmap_file, const char* environment_file);

	ga_reflective_lit_material(const char* texture_file, const char* normalmap_file, ga_cube_texture* env_map, const char* roughMap_file = "", const char* metalMap_file = "", const char* ao_file = "");

	virtual bool init() override;

	virtual void bindLight(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform, const struct ga_light_drawcall& lights, const ga_mat4f& shadowMVP) override;

	virtual float get_roughness();
	virtual void set_roughness(float roughness);

	virtual float get_metalness() { return _metalness; }
	virtual void set_metalness(float metalness) { _metalness = metalness; }

	virtual float get_normalStr() { return _normalStr; }
	virtual void set_normalStr(float normalStr) { _normalStr = normalStr; }

	bool get_useEnvMap() { return _useEnvMap; }
	bool get_useRoughMap() { return _useRoughMap; }
	bool get_useMetallicMap() { return _useMetalMap; }
	bool get_useAOMap() { return _useAOMap; }
	void switch_pbr_maps(bool useEnv, bool useRgh, bool useMtl, bool useAO) {
		_useEnvMap = useEnv;
		_useRoughMap = useRgh;
		_useMetalMap = useMtl;
		_useAOMap = useAO;
	}

	bool get_NDF() { return _NDF; }
	bool get_GEO() { return _GEO; }
	bool get_FNL() { return _FNL; }
	void switch_brdf_comp(bool ndf, bool geo, bool fnl);

	int get_debug_uniform() { return _debug_uniform; }
	void set_debug_uniform(int input) { _debug_uniform = input; }

	// Jay's Functions
	bool get_useRoughnessMap() { return _useRoughMap; };
	void set_useRoughnessMap(bool use) { _useRoughMap = use; };

	bool get_useMetallicMap() { return _useMetalMap; };
	void set_useMetallicMap(bool use) { _useMetalMap = use; };

	virtual ga_texture* get_roughnessMap() { return _roughnessMap; }
	virtual ga_texture* get_metallicMap() { return _metallicMap; }

protected:
	ga_cube_texture* _envMap;

	bool _useEnvMap;
	bool _useRoughMap;
	bool _useMetalMap;
	bool _useAOMap;

	float _roughness;
	float _metalness;
	float _normalStr;
	std::string _roughness_file;
	std::string _metallic_file;
	std::string _ao_file;
	ga_texture* _roughnessMap;
	ga_texture* _metallicMap;
	ga_texture* _aoMap;

	// spit steps for visualization
	bool _NDF;
	bool _GEO;
	bool _FNL;
	int _debug_uniform;
};


// TODO : so much overlap with ga_reflection, a common super-class is likely possible
class ga_refractive_lit_material : public ga_lit_material {
public:
	// ga_refractive_lit_material(const char* texture_file, const char* normalmap_file, const char* environment_file);
	ga_refractive_lit_material(const char* texture_file, const char* normalmap_file, ga_cube_texture* env_map, const char* roughMap_file, const char* metalMap_file);
	// ga_refractive_lit_material(const char* texture_file, const char* normalmap_file, ga_cube_texture* env_map);

	virtual bool init() override;

	virtual const char* get_name() override { return "refractive material"; }

	virtual float get_roughness() { return _roughness; }
	void bindLight(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform,
		const ga_light_drawcall& lights, const ga_mat4f& shadowMVP, int depthTexIndex, int normalTexIndex,
		const ga_mat4f& u_eyeProj, const ga_vec3f& eyePos, const ga_mat4f& vpMat_for_world_proj);
	virtual void set_roughness(float roughness) { _roughness = roughness; }

	virtual float get_metalness() { return _metalness; }
	virtual void set_metalness(float metalness) { _metalness = metalness; }

	virtual float get_normalStr() { return _normalStr; }
	virtual void set_normalStr(float normalStr) { _normalStr = normalStr; }

	virtual float get_IOR() { return _ior;  }
	virtual void set_IOR(float ior) { _ior = ior; }

	bool get_useRoughnessMap() { return _useRoughMap; };
	void set_useRoughnessMap(bool use) { _useRoughMap = use; };

	bool get_useMetallicMap() { return _useMetalMap; };
	void set_useMetallicMap(bool use) { _useMetalMap = use; };

	virtual ga_texture* get_normalMap() { return _normalmap; }
	virtual ga_texture* get_metallicMap() { return _metallicMap; }
	virtual ga_texture* get_roughnessMap() { return _roughnessMap; }
protected:
	ga_cube_texture * _envMap;

	bool _useEnvMap;

	bool _useRoughMap;
	bool _useMetalMap;

	float _roughness;
	float _metalness;
	float _normalStr;
	float _ior;

	std::string _roughness_file;
	std::string _metallic_file;
	ga_texture * _roughnessMap;
	ga_texture* _metallicMap;
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