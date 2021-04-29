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

unsigned char* convolveImage(unsigned char* data, int width, int height, int nChannels, int numConvolves = 2) {
	unsigned char* returnData = new unsigned char[width * height * nChannels];
	unsigned char * lastResult = new unsigned char[width * height * nChannels];

	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			for (int n = 0; n < nChannels; ++n) {
				int i = (h * width * nChannels) + (w * nChannels) + n;
				lastResult[i] = data[i];
			}
		}
	}
	/*
	double kernel[5][5] = { { 1.0 / 273.0, 4.0 / 273, 7.0 / 273,  4.0 / 273, 1.0 / 273},
							{ 4.0 / 273, 16.0 / 273, 26.0 / 273, 16.0 / 273, 4.0 / 273},
							{ 7.0 / 273, 26.0 / 273, 41.0 / 273, 26.0 / 273, 7.0 / 273},
							{ 4.0 / 273, 16.0 / 273, 26.0 / 273, 16.0 / 273, 4.0 / 273}, 
							{ 1.0 / 273, 4.0 / 273,   7.0 / 273,  4.0 / 273, 1.0 / 273} };
	int kernelSize = 5;
	*/
	
	double kernel[3][3] = { { 1.0 / 16, 2.0 / 16, 1.0 / 16},
							{ 2.0 / 16, 4.0 / 16, 2.0 / 16},
							{ 1.0 / 16, 2.0 / 16, 1.0 / 16} };
	int kernelSize = 3;
	
	for (int i = 0; i < numConvolves; ++i) {
		for (int h = 0; h < height; ++h) {
			for (int w = 0; w < width; ++w) {
				for (int n = 0; n < nChannels; ++n) {
					double val = 0;
					for (int h2 = 0; h2 < kernelSize; ++h2) {
						for (int w2 = 0; w2 < kernelSize; ++w2) {
							int hSamp = std::min(std::max((h + h2 - (kernelSize / 2)), 0), height - 1);
							int wSamp = std::min(std::max((w + w2 - (kernelSize / 2)), 0), width - 1);
							double pixel = (double)lastResult[(hSamp * width * nChannels) + (wSamp * nChannels) + n];
							val += (kernel[h2][w2] * pixel);
						}
					}
					returnData[((h)*width * nChannels) + ((w)*nChannels) + n] = (unsigned char)val;
				}
			}
		}
		unsigned char* temp = returnData;
		returnData = lastResult;
		lastResult = temp;
	}
	delete[] returnData;


	double* downSize = new double[(width/2) * (height/2) * nChannels];

	for (int h = 0; h < height/2; ++h) {
		for (int w = 0; w < width/2; ++w) {
			for (int n = 0; n < nChannels; ++n) {
				int i = (h * (width/2) * nChannels) + (w * nChannels) + n;
				downSize[i] = 0;

			}
		}
	}
	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			for (int n = 0; n < nChannels; ++n) {
				int downI = ((h / 2) * (width / 2) * nChannels) + ((w / 2) * nChannels) + n;
				int i = (h * width * nChannels) + (w * nChannels) + n;
				downSize[downI] += (double)lastResult[i] / 4.0;
			}
		}
	}

	unsigned char * downSizeChar = new unsigned char[(width / 2) * (height / 2) * nChannels];
	for (int h = 0; h < height/2; ++h) {
		for (int w = 0; w < width/2; ++w) {
			for (int n = 0; n < nChannels; ++n) {
				int i = (h * (width / 2) * nChannels) + (w * nChannels) + n;
				downSizeChar[i] = downSize[i];
			}
		}
	}
	delete[] lastResult;
	delete[] downSize;
	return downSizeChar;
}

void performConvolutionMipmapping(int i, unsigned char * data, int width, int height, int nrChannels, int convolutions=1) {

	unsigned char* newData1 = convolveImage(data, width, height, nrChannels, convolutions);
	unsigned char* newData2 = convolveImage(newData1, width / 2, height / 2, nrChannels, convolutions);
	unsigned char* newData3 = convolveImage(newData2, width / 4, height / 4, nrChannels, convolutions);
	unsigned char* newData4 = convolveImage(newData3, width / 8, height / 8, nrChannels, convolutions);
	unsigned char* newData5 = convolveImage(newData4, width / 16, height / 16, nrChannels, convolutions);
	unsigned char* newData6 = convolveImage(newData5, width / 32, height / 32, nrChannels, convolutions);
	unsigned char* newData7 = convolveImage(newData6, width / 64, height / 64, nrChannels, convolutions);
	unsigned char* newData8 = convolveImage(newData7, width / 128, height / 128, nrChannels, convolutions);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		1, GL_RGB, width / 2, height / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, newData1
	);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		2, GL_RGB, width / 4, height / 4, 0, GL_RGB, GL_UNSIGNED_BYTE, newData2
	);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		3, GL_RGB, width / 8, height / 8, 0, GL_RGB, GL_UNSIGNED_BYTE, newData3
	);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		4, GL_RGB, width / 16, height / 16, 0, GL_RGB, GL_UNSIGNED_BYTE, newData4
	);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		5, GL_RGB, width / 32, height / 32, 0, GL_RGB, GL_UNSIGNED_BYTE, newData5
	);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		6, GL_RGB, width / 64, height / 64, 0, GL_RGB, GL_UNSIGNED_BYTE, newData6
	);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		7, GL_RGB, width / 128, height / 128, 0, GL_RGB, GL_UNSIGNED_BYTE, newData7
	);
	/*glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
		8, GL_RGB, width / 256, height / 256, 0, GL_RGB, GL_UNSIGNED_BYTE, newData8
	);*/

	delete[] newData1;
	delete[] newData2;
	delete[] newData3;
	delete[] newData4;
	delete[] newData5;
	delete[] newData6;
	delete[] newData7;
}

uint32_t ga_cube_texture::load_cube_data(std::vector<std::string> faces)
{
	bool convolveMipmaps = true;
	stbi_set_flip_vertically_on_load(false);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 7);
	if (convolveMipmaps)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

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
			if (convolveMipmaps)
				performConvolutionMipmapping(i, data, width, height, nrChannels, 1);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap-tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	if (!convolveMipmaps)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return textureID;
}
