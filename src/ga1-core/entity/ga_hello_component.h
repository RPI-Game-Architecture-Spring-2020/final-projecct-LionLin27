#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_component.h"

#include <string>

/*
** A test component that generates an empty draw call.
*/
class ga_hello_component : public ga_component
{
public:
	ga_hello_component(class ga_entity* ent, const char* name);
	virtual ~ga_hello_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	std::string _name;
};
