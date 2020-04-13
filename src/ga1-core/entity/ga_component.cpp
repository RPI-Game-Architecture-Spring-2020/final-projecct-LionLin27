/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_component.h"

#include "ga_entity.h"

ga_component::ga_component(ga_entity* ent) : _entity(ent)
{
	_entity->add_component(this);
}

ga_component::~ga_component()
{
}

void ga_component::update(ga_frame_params* params)
{
}

void ga_component::late_update(ga_frame_params* params)
{
}
