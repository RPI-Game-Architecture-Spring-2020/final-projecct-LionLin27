/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec2f.h"

static ga_vec2f vec2f_zero = { 0.0f, 0.0f };
static ga_vec2f vec2f_one = { 1.0f, 1.0f };
static ga_vec2f vec2f_x = { 1.0f, 0.0f };
static ga_vec2f vec2f_y = { 0.0f, 1.0f };

ga_vec2f ga_vec2f::zero_vector()
{
	return vec2f_zero;
}

ga_vec2f ga_vec2f::one_vector()
{
	return vec2f_one;
}

ga_vec2f ga_vec2f::x_vector()
{
	return vec2f_x;
}

ga_vec2f ga_vec2f::y_vector()
{
	return vec2f_y;
}
