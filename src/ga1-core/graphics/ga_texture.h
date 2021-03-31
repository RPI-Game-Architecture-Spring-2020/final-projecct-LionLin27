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
#include <string>
#include <vector>
/*
** Represents a texture object for rendering.
*/
class ga_texture
{
	friend class ga_uniform;

public:
	ga_texture();
	~ga_texture();

	virtual void load_from_data(uint32_t width, uint32_t height, uint32_t channels, void* data);
	virtual bool load_from_file(const char* path);

	virtual uint32_t* get_handle() {
		return &_handle;
	}

protected:
	uint32_t _handle;
};

class ga_cube_texture : ga_texture{
public:
	ga_cube_texture(const char* path) { load_from_file(path); };
	virtual bool load_from_file(const char* path) override;
	virtual uint32_t* get_handle() override {
		return &_handle;
	}
private:
	uint32_t load_cube_data(std::vector<std::string> faces);
};