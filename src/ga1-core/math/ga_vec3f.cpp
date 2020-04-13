/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec3f.h"

static ga_vec3f vec3f_zero = { 0.0f, 0.0f, 0.0f };
static ga_vec3f vec3f_one = { 1.0f, 1.0f, 0.0f };
static ga_vec3f vec3f_x = { 1.0f, 0.0f, 0.0f };
static ga_vec3f vec3f_y = { 0.0f, 1.0f, 0.0f };
static ga_vec3f vec3f_z = { 0.0f, 0.0f, 1.0f };

ga_vec3f ga_vec3f::zero_vector()
{
	return vec3f_zero;
}

ga_vec3f ga_vec3f::one_vector()
{
	return vec3f_one;
}

ga_vec3f ga_vec3f::x_vector()
{
	return vec3f_x;
}

ga_vec3f ga_vec3f::y_vector()
{
	return vec3f_y;
}

ga_vec3f ga_vec3f::z_vector()
{
	return vec3f_z;
}
