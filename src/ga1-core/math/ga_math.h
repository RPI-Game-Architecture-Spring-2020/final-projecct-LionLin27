#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#define ga_absf fabsf
#define ga_cosf cosf
#define ga_powf powf
#define ga_sinf sinf
#define ga_sqrtf sqrtf
#define ga_tanf tanf
#define ga_max std::max
#define ga_min std::min

#define GA_PI ((float)M_PI)

/*
** Convert degrees to radians.
*/
inline float ga_degrees_to_radians(float degrees)
{
	return (degrees * GA_PI) / 180.0f;
}

/*
** Determine if two floats are largely equivalent.
*/
inline bool ga_equalf(float a, float b)
{
	float diff = ga_absf(a - b);
	return diff < 0.0000005f || diff < ga_absf(a * 0.0000005f) || diff < ga_absf(b * 0.0000005f);
}
