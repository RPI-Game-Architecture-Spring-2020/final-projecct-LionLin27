#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec3f.h"

/*
** Base class for GUI widgets.
*/
class ga_widget
{
protected:
	static const ga_vec3f k_button_color;
	static const ga_vec3f k_button_hover_color;
	static const ga_vec3f k_button_press_color;
	static const ga_vec3f k_text_color;
	static const float k_button_offset;
	static const float k_checkbox_offset;

	void draw_outline(struct ga_frame_params* params, const struct ga_vec2f& min, const struct ga_vec2f& max, const struct ga_vec3f& color, float offset);
	void draw_check(struct ga_frame_params* params, const struct ga_vec2f& min, const struct ga_vec2f& max, const struct ga_vec3f& color);
	void draw_fill(struct ga_frame_params* params, const struct ga_vec2f& min, const struct ga_vec2f& max, const struct ga_vec3f& color);
};
