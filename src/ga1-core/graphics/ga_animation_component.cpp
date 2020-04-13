/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_animation_component.h"

#include "ga_animation.h"
#include "ga_debug_geometry.h"
#include "ga_geometry.h"
#include "entity/ga_entity.h"

#include <cassert>

ga_animation_component::ga_animation_component(ga_entity* ent, ga_model* model) : ga_component(ent)
{
	_skeleton = model->_skeleton;
	assert(_skeleton != 0);
}

ga_animation_component::~ga_animation_component()
{
	_skeleton = 0;

	if (_playing)
	{
		delete _playing;
	}
}

void ga_animation_component::update(ga_frame_params* params)
{
	if (_playing)
	{
		_playing->_time += std::chrono::duration_cast<std::chrono::milliseconds>(params->_delta_time);

		float local_time = (_playing->_time.count() % 1000) / 1000.0f;
		uint32_t frame = (uint32_t)(local_time * _playing->_animation->_rate);
		// Safety.
		frame = frame % _playing->_animation->_rate;

		// For now, no interpolation. Select the closest frame.
		for (uint32_t joint_index = 0; joint_index < _skeleton->_joints.size(); ++joint_index)
		{
			ga_joint* j = _skeleton->_joints[joint_index];

			ga_mat4f parent_matrix;
			parent_matrix.make_identity();
			if (j->_parent < INT_MAX)
			{
				parent_matrix = _skeleton->_joints[j->_parent]->_world;
			}
			j->_world = _playing->_animation->_poses[frame]._transforms[joint_index] * parent_matrix;
			j->_skin = j->_inv_bind * j->_world;

#if DEBUG_DRAW_SKELETON
			ga_dynamic_drawcall drawcall;
			draw_debug_sphere(0.4f, j->_world * get_entity()->get_transform(), &drawcall);

			while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
			params->_dynamic_drawcalls.push_back(drawcall);
			params->_dynamic_drawcall_lock.clear(std::memory_order_release);
#endif
		}
	}
}

void ga_animation_component::play(ga_animation* animation)
{
	_playing = new ga_animation_playback();
	_playing->_animation = animation;
	_playing->_time == std::chrono::milliseconds::zero();
}
