#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <cstdint>

/*
** Represents a texture object for rendering.
*/
class ga_texture
{
	friend class ga_uniform;

public:
	ga_texture();
	~ga_texture();

	void load_from_data(uint32_t width, uint32_t height, uint32_t channels, void* data);
	bool load_from_file(const char* path);

private:
	uint32_t _handle;
};
