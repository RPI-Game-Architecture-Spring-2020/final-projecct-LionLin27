/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_camera.h"

#include "ga_frame_params.h"
#include "math/ga_math.h"

ga_camera::ga_camera(const ga_vec3f& eye)
{
	_transform.make_translation(eye);
}

ga_camera::~ga_camera()
{
}

void ga_camera::update(ga_frame_params* params)
{
	const float k_move_speed = 0.1f;
	const float k_rotate_speed = 0.5f;

	// focus
	if (params->_btn_down_mask & k_button_f) {
		ga_vec3f ent_pos = params->_selected_ent->get_transform().get_translation();
		float ent_size = params->_selected_ent->get_size().mag();
		ga_vec3f forward_dir = _transform.get_forward();
		float offset = 3.0f;

		ga_vec3f target_pos = ent_pos - forward_dir.scale_result(offset*ent_size);
		ga_vec3f move_dir = target_pos - _transform.get_translation();
		//move_dir.normalize();
		_transform.set_translation(_transform.get_translation() + move_dir.scale_result(0.2f));
	}

	// Use WASD to control the position.
	ga_vec3f translation = { 0.0f, 0.0f, 0.0f };
	translation += (params->_btn_down_mask & k_button_d) ? -_transform.get_right() : ga_vec3f::zero_vector();
	translation += (params->_btn_down_mask & k_button_a) ? _transform.get_right() : ga_vec3f::zero_vector();
	translation += (params->_btn_down_mask & k_button_s) ? -_transform.get_forward() : ga_vec3f::zero_vector();
	translation += (params->_btn_down_mask & k_button_w) ? _transform.get_forward() : ga_vec3f::zero_vector();
	translation += (params->_btn_down_mask & k_button_q) ? -_transform.get_up() : ga_vec3f::zero_vector();
	translation += (params->_btn_down_mask & k_button_e) ? _transform.get_up() : ga_vec3f::zero_vector();
	translation.scale(k_move_speed);

	// By using the camera's directional vectors, we've defined the translation in world space.
	ga_mat4f world_translation;
	world_translation.make_translation(translation);
	_transform = _transform * world_translation;

	// Use arrow keys to pitch and rotate.
	float rotation = 0.0f;
	rotation += (params->_btn_down_mask & k_button_left) ? k_rotate_speed : 0.0f;
	rotation += (params->_btn_down_mask & k_button_right) ? -k_rotate_speed : 0.0f;
	float pitch = 0.0f;
	pitch += (params->_btn_down_mask & k_button_up) ? -k_rotate_speed : 0.0f;
	pitch += (params->_btn_down_mask & k_button_down) ? k_rotate_speed : 0.0f;

	rotation = ga_degrees_to_radians(rotation);
	pitch = ga_degrees_to_radians(pitch);

	ga_quatf rotation_axis_angle;
	rotation_axis_angle.make_axis_angle(ga_vec3f::y_vector(), rotation);
	ga_mat4f rotation_matrix;
	rotation_matrix.make_rotation(rotation_axis_angle);

	rotation_axis_angle.make_axis_angle(ga_vec3f::x_vector(), pitch);
	rotation_matrix.rotate(rotation_axis_angle);
	_transform = rotation_matrix * _transform;

	ga_vec3f eye = _transform.get_translation();
	ga_vec3f at = eye + _transform.get_forward();
	ga_vec3f up = ga_vec3f::y_vector();

	ga_mat4f view;
	view.make_lookat_rh(eye, at, up);

	params->_view = view;
	params->_camPos = _transform.get_translation();
	params->_camDir = _transform.get_forward();
}

void ga_camera::rotate(const ga_quatf& rotation)
{
	ga_mat4f rotation_matrix;
	rotation_matrix.make_rotation(rotation);
	_transform = rotation_matrix * _transform;
}
