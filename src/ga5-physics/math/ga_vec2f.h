#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_vecnf.h"

/*
** Two component floating point vector.
*/
struct ga_vec2f
{
	union
	{
		struct { float x, y; };
		float axes[2];
	};

	GA_VECN_FUNCTIONS(2)

	static ga_vec2f zero_vector();
	static ga_vec2f one_vector();
	static ga_vec2f x_vector();
	static ga_vec2f y_vector();
};

#undef GA_VECN_FUNCTIONS
