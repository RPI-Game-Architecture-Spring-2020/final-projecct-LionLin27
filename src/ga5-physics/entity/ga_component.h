#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_frame_params.h"

/*
** Base class component object.
** All entity functionality is expected to derive from this object.
** @see ga_entity
*/
class ga_component
{
public:
	ga_component() = delete;
	ga_component(class ga_entity* ent);
	virtual ~ga_component();

	virtual void update(struct ga_frame_params* params);
	virtual void late_update(struct ga_frame_params* params);

	const class ga_entity* get_entity() const { return _entity; }
	class ga_entity* get_entity() { return _entity; }

private:
	class ga_entity* _entity;
};
