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
	_light = light;
}

ga_light_component::~ga_light_component()
{
}

void ga_light_component::update(ga_frame_params* params){
	if ((params->_btn_down_mask & k_button_c) != 0) {
		_light->_intensity += 0.1;
	}
	if ((params->_btn_down_mask & k_button_v) != 0) {
		_light->_intensity -= 0.1;
	}
	if ((params->_btn_down_mask & k_button_b) != 0) {
		if (_light->_direction.x > -1)
			_light->_direction -= {0.1, 0, 0};
		_light->_direction.normalize();
	}
	if ((params->_btn_down_mask & k_button_n) != 0) {
		if(_light->_direction.x < 1)
			_light->_direction += {0.1, 0, 0};
		_light->_direction.normalize();
	}
}
