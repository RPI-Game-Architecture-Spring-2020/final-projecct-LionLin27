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

#include "math/ga_mat4f.h"
#include "math/ga_vec3f.h"

#include <cassert>

#define GLEW_STATIC
#include <GL/glew.h>

void ga_uniform::set(const ga_vec3f& vec)
{
	glUniform3fv(_location, 1, vec.axes);
}

void ga_uniform::set(const ga_mat4f& mat)
{
	glUniformMatrix4fv(_location, 1, GL_TRUE, (const GLfloat*)mat.data);
}

void ga_uniform::set(const ga_mat4f* mats, uint32_t count)
{
	glUniformMatrix4fv(_location, count, GL_TRUE, (const GLfloat*)mats[0].data);
}

void ga_uniform::set(const ga_texture& tex, uint32_t unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, tex._handle);
	glUniform1i(_location, unit);
}

ga_uniform::ga_uniform(int32_t location) : _location(location) {}

ga_shader::ga_shader(const char* source, GLenum type)
{
	_handle = glCreateShader(type);
	glShaderSource(_handle, 1, &source, 0);
}

ga_shader::~ga_shader()
{
	glDeleteShader(_handle);
}

bool ga_shader::compile()
{
	glCompileShader(_handle);

	int32_t compile_status = GL_FALSE;
	glGetShaderiv(_handle, GL_COMPILE_STATUS, &compile_status);
	return compile_status == GL_TRUE;
}

std::string ga_shader::get_compile_log() const
{
	int32_t length;
	glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &length);

	std::string log(length, '\0');
	glGetShaderInfoLog(_handle, length, &length, &log[0]);
	return log;
}

ga_program::ga_program()
{
	_handle = glCreateProgram();
}

ga_program::~ga_program()
{
	glDeleteProgram(_handle);
}

void ga_program::attach(const ga_shader& shader)
{
	glAttachShader(_handle, shader._handle);
}

void ga_program::detach(const ga_shader& shader)
{
	glDetachShader(_handle, shader._handle);
}

bool ga_program::link()
{
	glLinkProgram(_handle);

	int32_t link_status = GL_FALSE;
	glGetProgramiv(_handle, GL_LINK_STATUS, &link_status);
	return link_status == GL_TRUE;
}

std::string ga_program::get_link_log() const
{
	int32_t length;
	glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &length);

	std::string log(length, '\0');
	glGetProgramInfoLog(_handle, length, &length, &log[0]);
	return log;
}

ga_uniform ga_program::get_uniform(const char* name)
{
	int32_t location = glGetUniformLocation(_handle, name);
	return ga_uniform(location);
}

void ga_program::use()
{
	glUseProgram(_handle);
}
