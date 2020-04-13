#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "entity/ga_component.h"

/*
** A component that adds physics simulation to an entity.
** Owns a rigid body and synchronizes its transform and that of the entity.
*/
class ga_physics_component : public ga_component
{
public:
	ga_physics_component(class ga_entity* ent, struct ga_shape* shape, float mass);
	virtual ~ga_physics_component();

	virtual void update(struct ga_frame_params* params) override;
	virtual void late_update(struct ga_frame_params* params) override;

	class ga_rigid_body* get_rigid_body() const { return _body; }

private:
	class ga_rigid_body* _body;
};
