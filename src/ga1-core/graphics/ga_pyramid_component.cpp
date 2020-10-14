/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_pyramid_component.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>

ga_pyramid_component::ga_pyramid_component(ga_entity* ent) : ga_component(ent)
{
	_material = new ga_custom_shader_material();
	const char* vs_address = "data/shaders/ga_playground_vert.glsl";
	const char* fs_address = "data/shaders/ga_playground_frag.glsl";
	_material->init(vs_address, fs_address);
	_material->SetTexture("data/textures/rpi.png");

	static GLfloat pyramidPositions[54] =
	{ -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
		1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
		-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
	};
	float textureCoordinates[36] =
	{ 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	_index_count = uint32_t(sizeof(pyramidPositions) / sizeof(*pyramidPositions));

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(_numVBOs, _vbos);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

ga_pyramid_component::~ga_pyramid_component()
{
	glDeleteBuffers(_numVBOs, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void ga_pyramid_component::update(ga_frame_params* params)
{
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	ga_quatf axis_angle;
	axis_angle.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(60.0f) * dt);
	get_entity()->rotate(axis_angle);

	ga_static_drawcall draw;
	draw._name = "ga_pyramid_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;
	draw._drawBuffer = true;

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw);
	params->_static_drawcall_lock.clear(std::memory_order_release);
}
