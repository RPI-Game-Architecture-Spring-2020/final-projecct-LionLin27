#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_mat4f.h"
#include "math/ga_vec2f.h"
#include "math/ga_vec3f.h"

#include <string>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

/*
** A draw emitted from the simulation phase and rendered in the output phase.
** @see ga_frame_params
*/
struct ga_drawcall
{
	std::string _name;
	ga_mat4f _transform;
	GLenum _draw_mode;
	class ga_material* _material = 0;
};

/*
** Draw call with static geometry.
** The vertex array object referenced by this draw call should live for at
** least several frames, and probably longer.
*/
struct ga_static_drawcall : ga_drawcall
{
	GLuint _vao;
	GLsizei _index_count;
};

/*
** Draw call with dynamic geometry.
** Geometry referenced by this draw call should only a single frame.
*/
struct ga_dynamic_drawcall : ga_drawcall
{
	std::vector<ga_vec3f> _positions;
	std::vector<ga_vec2f> _texcoords;
	std::vector<uint16_t> _indices;
	ga_vec3f _color;
};
