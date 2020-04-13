/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_button.h"
#include "ga_font.h"

#include "framework/ga_frame_params.h"

ga_button::ga_button(const char* text, float x, float y, ga_frame_params* params)
{
	extern ga_font* g_font;
	g_font->print(params, text, x, y, k_text_color, &_min, &_max);

	if (get_hover(params))
	{
		draw_outline(params, _min, _max, k_button_hover_color, k_button_offset);
	}
	else
	{
		draw_outline(params, _min, _max, k_button_color, k_button_offset);
	}

	if (get_pressed(params))
	{
		draw_fill(params, _min, _max, k_button_press_color);
		g_font->print(params, text, x, y, k_text_color);
	}
}

ga_button::~ga_button()
{
}

bool ga_button::get_hover(const ga_frame_params* params) const
{
	return
		_min.x <= params->_mouse_x &&
		_min.y <= params->_mouse_y &&
		_max.x >= params->_mouse_x &&
		_max.y >= params->_mouse_y;
}

bool ga_button::get_pressed(const ga_frame_params* params) const
{
	return params->_mouse_press_mask != 0 && get_hover(params);
}

bool ga_button::get_clicked(const ga_frame_params* params) const
{
	return params->_mouse_click_mask != 0 && get_hover(params);
}
