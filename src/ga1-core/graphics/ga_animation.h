#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_mat4f.h"

#include <chrono>
#include <climits>
#include <vector>

struct ga_joint
{
	ga_mat4f _world;
	ga_mat4f _inv_bind;
	ga_mat4f _skin;

	uint32_t _parent = INT_MAX;
};

struct ga_skeleton
{
	static const uint32_t k_max_skeleton_joints = 30;

	std::vector<ga_joint*> _joints;
};

struct ga_skeleton_pose
{
	std::vector<ga_mat4f> _transforms;
};

struct ga_animation
{
	float _length;
	uint32_t _rate;

	std::vector<ga_skeleton_pose> _poses;
};

struct ga_animation_playback
{
	ga_animation* _animation;
	std::chrono::milliseconds _time;
};
