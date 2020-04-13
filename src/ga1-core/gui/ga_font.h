#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "graphics/ga_material.h"
#include "math/ga_vec3f.h"

#include <stb_truetype.h>

/*
** Represents a TrueType font of a certain pixel height.
** Capable of displaying ASCII characters from that font on screen.
*/
class ga_font
{
public:
	ga_font(const char* path, float char_height, int image_width, int image_height);
	~ga_font();

	void print(
		struct ga_frame_params* params,
		const char* text,
		float x,
		float y,
		const struct ga_vec3f& color,
		struct ga_vec2f* min = nullptr,
		struct ga_vec2f* max = nullptr);

private:
	class ga_texture* _texture;
	class ga_font_material* _material;

	int _image_width;
	int _image_height;

	enum { k_character_start = 32, k_character_count = 96 };
	stbtt_bakedchar _characters[k_character_count];
};

/*
** Material used to render ga_font.
*/
class ga_font_material : public ga_material
{
public:
	ga_font_material(ga_texture* texture);
	~ga_font_material();

	virtual bool init() override;

	virtual void bind(const struct ga_mat4f& view_proj, const struct ga_mat4f& transform) override;

	virtual void set_color(const struct ga_vec3f& color) override { _color = color; }

private:
	class ga_shader* _vs;
	class ga_shader* _fs;
	class ga_program* _program;
	class ga_texture* _texture;
	ga_vec3f _color;
};
