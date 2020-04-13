/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_label.h"
#include "ga_font.h"

#include "framework/ga_frame_params.h"

ga_label::ga_label(const char* text, float x, float y, ga_frame_params* params)
{
	extern ga_font* g_font;
	g_font->print(params, text, x, y, k_text_color);
}

ga_label::~ga_label()
{
}
