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

/*
** A component whose logic is implemented in LUA.
*/
class ga_lua_component : public ga_component
{
public:
	ga_lua_component(class ga_entity* ent, const char* path);
	virtual ~ga_lua_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	static int lua_frame_params_get_input_left(struct lua_State* state);
	static int lua_frame_params_get_input_right(struct lua_State* state);
	static int lua_component_get_entity(struct lua_State* state);
	static int lua_entity_translate(struct lua_State* state);

	struct lua_State* _lua;
};
