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
** A component that drives entity transform through user input.
*/
class ga_playermove_component : public ga_component
{
public:
	ga_playermove_component(class ga_entity* ent);
	virtual ~ga_playermove_component();

	virtual void update(struct ga_frame_params* params) override;

	void set_move_when_paused(bool state);

private:
	bool _move_when_paused;
};
