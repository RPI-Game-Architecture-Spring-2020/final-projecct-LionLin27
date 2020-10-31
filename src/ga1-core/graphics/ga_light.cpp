/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_light.h"
#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>

ga_light::ga_light(ga_vec3f color, float intensity)
{
	_color = color;
	_intensity = intensity;
}

ga_directional_light::ga_directional_light(ga_vec3f color, float intensity, ga_vec3f direction)
	:ga_light(color, intensity)
{
	direction.normalize();
	_direction = direction;
}

ga_positional_light::ga_positional_light(ga_vec3f color, float intensity, ga_vec3f position)
	:ga_light(color, intensity)
{
	_position = position;
}
