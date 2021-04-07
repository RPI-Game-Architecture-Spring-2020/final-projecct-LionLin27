/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/
#include <cassert>

#include "ga_model_component.h"

#include "ga_animation.h"
#include "ga_geometry.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>

ga_model_component::ga_model_component(ga_entity* ent, ga_model* model, ga_material* mat, bool lit) : ga_component(ent)
{
	_lit = lit;
	_material = mat;
	_material->init();
	bind_model(model);
}

ga_model_component::ga_model_component(ga_entity* ent, ga_patch* model, ga_material* mat, bool lit) : ga_component(ent)
{
	_lit = lit;
	_material = mat;
	_material->init();

	_drawPatch = true;
	((ga_tess_plane_material*)mat)->bindPatch(model);

	bind_patch(model);
}

ga_model_component::ga_model_component(ga_entity* ent, ga_terrain* model, ga_material* mat, bool lit) : ga_component(ent)
{
	_drawTerrain = true;

	_lit = lit;
	_material = mat;
	_material->init();

	_terrain = model;
	((ga_terrain_material*)mat)->bindTerrain(model);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
}

ga_model_component::ga_model_component(ga_entity* ent, ga_model* model, const char* texturePath) : ga_component(ent)
{
	//_material = new ga_lit_anim_material(model->_skeleton, _light);
	_material = new ga_unlit_texture_material(texturePath);
	//_material = new ga_constant_color_material();
	//_material->set_color({1.0f, 1.0f, 1.0});

	_light = NULL;
	_drawBuffer = true; // TODO: currently obj loader does not support draw element
	_material->init();

	bind_model(model);
}

ga_model_component::ga_model_component(ga_entity* ent, ga_model* model, ga_directional_light* light) : ga_component(ent)
{
	_light = light;
	_lit = true;
	_material = new ga_lit_anim_material(model->_skeleton, _light);
	_material->init();

	bind_model(model);
}

ga_model_component::~ga_model_component()
{
	glDeleteBuffers(2, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void ga_model_component::update(ga_frame_params* params)
{
	ga_static_drawcall draw;
	draw._name = "ga_model_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;
	draw._drawBuffer = _drawBuffer;
	draw._drawPatch = _drawPatch;
	draw._drawTerrain = _drawTerrain;
	draw._lit = _lit;

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw);
	params->_static_drawcall_lock.clear(std::memory_order_release);
}

ga_patch* ga_model_component::get_patch()
{
	return _patch;
}

ga_terrain* ga_model_component::get_terrain() {
	return _terrain;
}

ga_material* ga_model_component::get_material()
{
	return _material;
}

void ga_model_component::bind_model(ga_model* model)
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(2, _vbos);

	GLsizei vertex_size = sizeof(model->_vertices[0]);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, model->_vertices.size() * vertex_size, &model->_vertices[0], GL_STATIC_DRAW);

	// Position first.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, 0);
	glEnableVertexAttribArray(0);

	if (model->_vertex_format & k_vertex_attribute_normal)
	{
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)12);
		glEnableVertexAttribArray(1);
	}

	if (model->_vertex_format & k_vertex_attribute_color)
	{
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)24);
		glEnableVertexAttribArray(2);
	}

	if (model->_vertex_format & k_vertex_attribute_uv)
	{
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)36);
		glEnableVertexAttribArray(3);
	}

	if (model->_vertex_format & k_vertex_attribute_weight)
	{
		assert(false);
		glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, vertex_size, (GLvoid*)44);
		glEnableVertexAttribArray(4);

		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)60);
		glEnableVertexAttribArray(5);
	}

	if (model->_vertex_format & k_vertex_attribute_tangent) {
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)44);
		glEnableVertexAttribArray(4);

	}
	if (model->_vertex_format & k_vertex_attribute_internal_dist) {
		// TODO
		glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)88);
		glEnableVertexAttribArray(6);
	}

	if (model->_indices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->_indices.size() * sizeof(model->_indices[0]), &model->_indices[0], GL_STATIC_DRAW);
		_index_count = (uint32_t)model->_indices.size();
		_drawBuffer = false;
	}
	else {
		_index_count = (uint32_t)model->_vertices.size();
		_drawBuffer = true;
	}
}

void ga_model_component::bind_patch(ga_patch* model) {
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_patch = model;
}
