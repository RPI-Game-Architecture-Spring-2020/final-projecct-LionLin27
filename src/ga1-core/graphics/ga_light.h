#pragma once

/*
** RPI Game Architecture Engine
**
*/

#include "entity/ga_component.h"

#include <cstdint>
#include "math/ga_vec3f.h"

class ga_light
{
public:
	ga_light(ga_vec3f color, float intensity);
	ga_vec3f _color;
	float _intensity;
};

class ga_directional_light : public ga_light {
public:
	ga_directional_light(ga_vec3f color, float intensity, ga_vec3f direction);
	ga_vec3f _direction;
};

class ga_positional_light : public ga_light {
public:
	ga_positional_light(ga_vec3f color, float intensity, ga_vec3f position);
	ga_vec3f _position;
};

