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

#define DEBUG_DRAW_SKELETON 1

/*
** Component which drives animation; updates skeleton and skinning matrices.
*/
class ga_animation_component : public ga_component
{
public:
	ga_animation_component(class ga_entity* ent, struct ga_model* model);
	virtual ~ga_animation_component();

	virtual void update(struct ga_frame_params* params) override;

	void play(struct ga_animation* animation);

private:
	struct ga_skeleton* _skeleton = 0;
	struct ga_animation_playback* _playing = 0;
};
