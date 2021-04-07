/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_light.h"
#include "ga_light_component.h"
#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>

ga_light_component::ga_light_component(ga_entity* ent, ga_directional_light* light) : ga_component(ent)
{
	_type = LightType::directional;
	_light = light;
}

ga_light_component::ga_light_component(ga_entity* ent, ga_positional_light* light) : ga_component(ent)
{
	_type = LightType::positional;
	_light = light;
}

//ga_light_component::~ga_light_component()
//{}

void ga_light_component::update(ga_frame_params* params){
	if (_type == LightType::directional) {
		ga_directional_light* dirL = static_cast<ga_directional_light*>(_light);
		dirL->_direction = _entity->get_transform().get_forward();
		
		while (params->_lights_lock.test_and_set(std::memory_order_acquire)) {}
		params->_lights._dirLight = dirL;
		params->_lights_lock.clear(std::memory_order_release);
	}
	else if (_type == LightType::positional) {
		ga_positional_light* posL = static_cast<ga_positional_light*>(_light);
		posL->_position = _entity->get_transform().get_translation();

		while (params->_lights_lock.test_and_set(std::memory_order_acquire)) {}
		params->_lights._posLightArr.push_back(posL);
		params->_lights_lock.clear(std::memory_order_release);
	}

}
