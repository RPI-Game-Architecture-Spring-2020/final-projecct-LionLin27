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
#include <vector>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <math/ga_mat4f.h>

ga_texture::ga_texture()
{
	stbi_set_flip_vertically_on_load(true);
	glGenTextures(1, &_handle);
}

ga_texture::~ga_texture()
{
	glDeleteTextures(1, &_handle);
}

void ga_texture::load_from_data(uint32_t width, uint32_t height, uint32_t channels, void* data)
{
	glBindTexture(GL_TEXTURE_2D, _handle);

	// mipmapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// AF
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
		GLfloat anisoSetting = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting);
	}

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

bool ga_cube_texture::load_from_file(const char* path)
{
	std::vector<std::string> mapVec;
	std::string xp = path; xp = xp + "/xp.jpg"; mapVec.push_back(xp);
	std::string xn = path; xn = xn + "/xn.jpg"; mapVec.push_back(xn);
	std::string yp = path; yp = yp + "/yp.jpg"; mapVec.push_back(yp);
	std::string yn = path; yn = yn + "/yn.jpg"; mapVec.push_back(yn);
	std::string zp = path; zp = zp + "/zp.jpg"; mapVec.push_back(zp);
	std::string zn = path; zn = zn + "/zn.jpg"; mapVec.push_back(zn);
	_handle = load_cube_data(mapVec);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	return false;
}

uint32_t ga_cube_texture::load_cube_data(std::vector<std::string> faces)
{
	stbi_set_flip_vertically_on_load(false);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 7);

	extern char g_root_path[256];
	std::string fullpath = g_root_path;

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::string path = fullpath + faces[i];
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// TODO : perform better convolutions

	return textureID;
}
