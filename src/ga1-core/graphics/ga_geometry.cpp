/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_geometry.h"

#include "ga_animation.h"

ga_model::ga_model()
{
}

ga_model::~ga_model()
{
	if (_skeleton)
	{
		delete _skeleton;
	}
}