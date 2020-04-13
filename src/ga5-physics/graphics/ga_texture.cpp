/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_texture.h"

#include <stb_image.h>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

ga_texture::ga_texture()
{
	glGenTextures(1, &_handle);
}

ga_texture::~ga_texture()
{
	glDeleteTextures(1, &_handle);
}

void ga_texture::load_from_data(uint32_t width, uint32_t height, uint32_t channels, void* data)
{
	glBindTexture(GL_TEXTURE_2D, _handle);
	glTexStorage2D(GL_TEXTURE_2D, 1, channels == 4 ? GL_RGBA8 : GL_R8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, channels == 4 ? GL_RGBA : GL_RED, GL_UNSIGNED_BYTE, data);
}

bool ga_texture::load_from_file(const char* path)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	int width, height, channels_in_file;
	uint8_t* data = stbi_load(fullpath.c_str(), &width, &height, &channels_in_file, 4);
	if (!data)
	{
		return false;
	}

	load_from_data(width, height, 4, data);

	stbi_image_free(data);

	return true;
}
