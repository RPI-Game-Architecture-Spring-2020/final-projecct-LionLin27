/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec4f.h"

static ga_vec4f vec4f_zero = { 0.0f, 0.0f, 0.0f, 0.0f };
static ga_vec4f vec4f_one = {  1.0f, 1.0f, 1.0f, 1.0f };
static ga_vec4f vec4f_x = { 1.0f, 0.0f, 0.0f, 0.0f };
static ga_vec4f vec4f_y = { 0.0f, 1.0f, 0.0f, 0.0f };
static ga_vec4f vec4f_z = { 0.0f, 0.0f, 1.0f, 0.0f };
static ga_vec4f vec4f_w = { 0.0f, 0.0f, 0.0f, 1.0f };

ga_vec4f ga_vec4f::zero_vector()
{
	return vec4f_zero;
}

ga_vec4f ga_vec4f::one_vector()
{
	return vec4f_one;
}

ga_vec4f ga_vec4f::x_vector()
{
	return vec4f_x;
}

ga_vec4f ga_vec4f::y_vector()
{
	return vec4f_y;
}

ga_vec4f ga_vec4f::z_vector()
{
	return vec4f_z;
}

ga_vec4f ga_vec4f::w_vector()
{
	return vec4f_w;
}
