/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_model_component.h"

#include "ga_animation.h"
#include "ga_geometry.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>

ga_model_component::ga_model_component(ga_entity* ent, ga_model* model, const char* texturePath) : ga_component(ent)
{
	//_material = new ga_lit_anim_material(model->_skeleton, _light);
	_material = new ga_unlit_texture_material(texturePath);
	//_material = new ga_constant_color_material();
	//_material->set_color({1.0f, 1.0f, 1.0});

	_light = NULL;
	_drawBuffer = true; // TODO: currently obj loader does not support draw element
	_material->init();

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
		glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, vertex_size, (GLvoid*)44);
		glEnableVertexAttribArray(4);

		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)60);
		glEnableVertexAttribArray(5);
	}


	if (model->_indices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->_indices.size() * sizeof(model->_indices[0]), &model->_indices[0], GL_STATIC_DRAW);
		_index_count = (uint32_t)model->_indices.size();
	}
	else {
		_index_count = (uint32_t)model->_vertices.size();
	}

	glBindVertexArray(0);
}

ga_model_component::ga_model_component(ga_entity* ent, ga_model* model, ga_directional_light* light) : ga_component(ent)
{
	_light = light;
	_material = new ga_lit_anim_material(model->_skeleton, _light);
	_material->init();

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
		glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, vertex_size, (GLvoid*)44);
		glEnableVertexAttribArray(4);

		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vertex_size, (GLvoid*)60);
		glEnableVertexAttribArray(5);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->_indices.size() * sizeof(model->_indices[0]), &model->_indices[0], GL_STATIC_DRAW);

	_index_count = (uint32_t)model->_indices.size();

	glBindVertexArray(0);
}

ga_model_component::~ga_model_component()
{
	glDeleteBuffers(2, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void ga_model_component::update(ga_frame_params* params)
{
	/*
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	ga_quatf axis_angle;
	axis_angle.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(60.0f) * dt);
	get_entity()->rotate(axis_angle);
	*/

	ga_static_drawcall draw;
	draw._name = "ga_animated_model_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;
	draw._drawBuffer = _drawBuffer;

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw);
	params->_static_drawcall_lock.clear(std::memory_order_release);
}
