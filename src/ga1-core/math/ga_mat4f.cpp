/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_mat4f.h"

#include "math/ga_math.h"

//#define GA_CLIP_SPACE_DX 1
#define GA_CLIP_SPACE_GL 1

void ga_mat4f::make_identity()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			data[i][j] = i == j ? 1.0f : 0.0f;
		}
	}
}

void ga_mat4f::make_translation(const ga_vec3f& __restrict t)
{
	make_identity();
	data[3][0] = t.x;
	data[3][1] = t.y;
	data[3][2] = t.z;
	data[3][3] = 1.0f;
}

void ga_mat4f::make_scaling(float s)
{
	make_identity();
	for (int i = 0; i < 3; ++i)
	{
		data[i][i] = s;
	}
	data[3][3] = 1.0f;
}

void ga_mat4f::make_rotation(const ga_quatf& __restrict q)
{
	make_identity();

	data[0][0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	data[1][0] = 2.0f * (q.x * q.y - q.z * q.w);
	data[2][0] = 2.0f * (q.x * q.z + q.y * q.w);
	data[0][1] = 2.0f * (q.x * q.y + q.z * q.w);
	data[1][1] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
	data[2][1] = 2.0f * (q.y * q.z - q.x * q.w);
	data[0][2] = 2.0f * (q.x * q.z - q.y * q.w);
	data[1][2] = 2.0f * (q.y * q.z + q.x * q.w);
	data[2][2] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	data[3][3] = 1.0f;
}

void ga_mat4f::translate(const ga_vec3f& __restrict t)
{
	ga_mat4f tmp;
	tmp.make_translation(t);
	(*this) *= tmp;
}

void ga_mat4f::scale(float s)
{
	ga_mat4f tmp;
	tmp.make_scaling(s);
	(*this) *= tmp;
}

void ga_mat4f::rotate(const ga_quatf& __restrict q)
{
	ga_mat4f tmp;
	tmp.make_rotation(q);
	(*this) *= tmp;
}

ga_mat4f ga_mat4f::operator*(const ga_mat4f& __restrict b) const
{
	ga_mat4f result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			float tmp = 0.0f;
			for (int k = 0; k < 4; ++k)
			{
				tmp += data[i][k] * b.data[k][j];
			}
			result.data[i][j] = tmp;
		}
	}
	return result;
}

ga_mat4f& ga_mat4f::operator*=(const ga_mat4f& __restrict m)
{
	(*this) = (*this) * m;
	return (*this);
}

ga_vec4f ga_mat4f::transform(const ga_vec4f& __restrict in) const
{
	ga_vec4f result;
	result.x = in.x * data[0][0] + in.y * data[1][0] + in.z * data[2][0] + in.w * data[3][0];
	result.y = in.x * data[0][1] + in.y * data[1][1] + in.z * data[2][1] + in.w * data[3][1];
	result.z = in.x * data[0][2] + in.y * data[1][2] + in.z * data[2][2] + in.w * data[3][2];
	result.w = in.x * data[0][3] + in.y * data[1][3] + in.z * data[2][3] + in.w * data[3][3];
	return result;
}

ga_vec3f ga_mat4f::transform_vector(const ga_vec3f& __restrict in) const
{
	ga_vec4f temp = { in.x, in.y, in.z, 0.0f };
	temp = transform(temp);
	return { temp.x, temp.y, temp.z };
}

ga_vec3f ga_mat4f::transform_point(const ga_vec3f& __restrict in) const
{
	ga_vec4f temp = { in.x, in.y, in.z, 1.0f };
	temp = transform(temp);
	return{ temp.x, temp.y, temp.z };
}

void ga_mat4f::transpose()
{
	ga_mat4f tmp;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			tmp.data[i][j] = data[j][i];
		}
	}
	(*this) = tmp;
}

void ga_mat4f::invert()
{
	float s[6];
	s[0] = data[0][0] * data[1][1] - data[1][0] * data[0][1];
	s[1] = data[0][0] * data[1][2] - data[1][0] * data[0][2];
	s[2] = data[0][0] * data[1][3] - data[1][0] * data[0][3];
	s[3] = data[0][1] * data[1][2] - data[1][1] * data[0][2];
	s[4] = data[0][1] * data[1][3] - data[1][1] * data[0][3];
	s[5] = data[0][2] * data[1][3] - data[1][2] * data[0][3];

	float c[6];
	c[0] = data[2][0] * data[3][1] - data[3][0] * data[2][1];
	c[1] = data[2][0] * data[3][2] - data[3][0] * data[2][2];
	c[2] = data[2][0] * data[3][3] - data[3][0] * data[2][3];
	c[3] = data[2][1] * data[3][2] - data[3][1] * data[2][2];
	c[4] = data[2][1] * data[3][3] - data[3][1] * data[2][3];
	c[5] = data[2][2] * data[3][3] - data[3][2] * data[2][3];

	float inv_det = s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0];
	//VLOG_ASSERT(inv_det != 0.0f, k_vlog_error, 100, "mat4f", "Attempting to invert matrix with zero determinant.");
	inv_det = 1.0f / inv_det;

	ga_mat4f tmp;
	tmp.data[0][0] = (data[1][1] * c[5] - data[1][2] * c[4] + data[1][3] * c[3])  * inv_det;
	tmp.data[0][1] = (-data[0][1] * c[5] + data[0][2] * c[4] - data[0][3] * c[3]) * inv_det;
	tmp.data[0][2] = (data[3][1] * s[5] - data[3][2] * s[4] + data[3][3] * s[3])  * inv_det;
	tmp.data[0][3] = (-data[2][1] * s[5] + data[2][2] * s[4] - data[2][3] * s[3]) * inv_det;

	tmp.data[1][0] = (-data[1][0] * c[5] + data[1][2] * c[2] - data[1][3] * c[1]) * inv_det;
	tmp.data[1][1] = (data[0][0] * c[5] - data[0][2] * c[2] + data[0][3] * c[1])  * inv_det;
	tmp.data[1][2] = (-data[3][0] * s[5] + data[3][2] * s[2] - data[3][3] * s[1]) * inv_det;
	tmp.data[1][3] = (data[2][0] * s[5] - data[2][2] * s[2] + data[2][3] * s[1])  * inv_det;

	tmp.data[2][0] = (data[1][0] * c[4] - data[1][1] * c[2] + data[1][3] * c[0])  * inv_det;
	tmp.data[2][1] = (-data[0][0] * c[4] + data[0][1] * c[2] - data[0][3] * c[0]) * inv_det;
	tmp.data[2][2] = (data[3][0] * s[4] - data[3][1] * s[2] + data[3][3] * s[0])  * inv_det;
	tmp.data[2][3] = (-data[2][0] * s[4] + data[2][1] * s[2] - data[2][3] * s[0]) * inv_det;

	tmp.data[3][0] = (-data[1][0] * c[3] + data[1][1] * c[1] - data[1][2] * c[0]) * inv_det;
	tmp.data[3][1] = (data[0][0] * c[3] - data[0][1] * c[1] + data[0][2] * c[0])  * inv_det;
	tmp.data[3][2] = (-data[3][0] * s[3] + data[3][1] * s[1] - data[3][2] * s[0]) * inv_det;
	tmp.data[3][3] = (data[2][0] * s[3] - data[2][1] * s[1] + data[2][2] * s[0])  * inv_det;

	(*this) = tmp;
}

ga_mat4f ga_mat4f::inverse() const
{
	ga_mat4f inverse = (*this);
	inverse.invert();
	return inverse;
}

void ga_mat4f::make_orthographic(float left, float right, float bottom, float top, float z_near, float z_far)
{
	float inv_width = 1.0f / (right - left);
	data[0][0] = 2.0f * inv_width;
	data[1][0] = 0.0f;
	data[2][0] = 0.0f;
	data[3][0] = -(right + left) * inv_width;

	float inv_height = 1.0f / (top - bottom);
	data[0][1] = 0.0f;
	data[1][1] = 2.0f * inv_height;
	data[2][1] = 0.0f;
	data[3][1] = -(top + bottom) * inv_height;

	float inv_depth = 1.0f / (z_near - z_far);
	data[0][2] = 0.0f;
	data[1][2] = 0.0f;
#if defined(GA_CLIP_SPACE_DX)
	data[2][2] = 1.0f * inv_depth;
	data[3][2] = z_near * inv_depth;
#elif defined(GA_CLIP_SPACE_GL)
	data[2][2] = 2.0f * inv_depth;
	data[3][2] = (z_far + z_near) * inv_depth;
#else
	#error "Unknown clip space."
#endif

	data[0][3] = 0.0f;
	data[1][3] = 0.0f;
	data[2][3] = 0.0f;
	data[3][3] = 1.0f;
}

void ga_mat4f::make_perspective_rh(float angle, float aspect, float z_near, float z_far)
{
	float a = 1.0f / ga_tanf(angle * 0.5f);

	data[0][0] = a / aspect;
	data[0][1] = 0.0f;
	data[0][2] = 0.0f;
	data[0][3] = 0.0f;

	data[1][0] = 0.0f;
	data[1][1] = a;
	data[1][2] = 0.0f;
	data[1][3] = 0.0f;

	data[2][0] = 0.0f;
	data[2][1] = 0.0f;
#if defined(GA_CLIP_SPACE_DX)
	data[2][2] = -z_far / (z_far - z_near);
#elif defined(GA_CLIP_SPACE_GL)
	data[2][2] = -(z_far + z_near) / (z_far - z_near);
#else
	#error "Unknown clip space."
#endif
	data[2][3] = -1.0f;

	data[3][0] = 0.0f;
	data[3][1] = 0.0f;
#if defined(GA_CLIP_SPACE_DX)
	data[3][2] = -(1.0f * z_far * z_near) / (z_far - z_near);
#elif defined(GA_CLIP_SPACE_GL)
	data[3][2] = -(2.0f * z_far * z_near) / (z_far - z_near);
#else
	#error "Unknown clip space."
#endif
	data[3][3] = 0.0f;
}

void ga_mat4f::make_lookat_rh(const ga_vec3f& __restrict eye, const ga_vec3f& __restrict at, const ga_vec3f& __restrict up)
{
	ga_vec3f z_vec = eye - at;
	z_vec.normalize();

	ga_vec3f x_vec = ga_vec3f_cross(up, z_vec);
	x_vec.normalize();

	ga_vec3f y_vec = ga_vec3f_cross(z_vec, x_vec);
	y_vec.normalize();

	// Row 0
	data[0][0] = x_vec.x;
	data[0][1] = y_vec.x;
	data[0][2] = z_vec.x;
	data[0][3] = 0.0f;

	// Row 1
	data[1][0] = x_vec.y;
	data[1][1] = y_vec.y;
	data[1][2] = z_vec.y;
	data[1][3] = 0.0f;

	// Row 2
	data[2][0] = x_vec.z;
	data[2][1] = y_vec.z;
	data[2][2] = z_vec.z;
	data[2][3] = 0.0f;

	// Row 3
	data[3][0] = -x_vec.dot(eye);
	data[3][1] = -y_vec.dot(eye);
	data[3][2] = -z_vec.dot(eye);
	data[3][3] = 1.0f;
}

bool ga_mat4f::equal(const ga_mat4f& __restrict b)
{
	bool is_not_equal = false;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			is_not_equal = is_not_equal || !ga_equalf(data[i][j], b.data[i][j]);
		}
	}
	return !is_not_equal;
}

ga_vec3f ga_mat4f::get_translation() const
{
	return { data[3][0], data[3][1], data[3][2] };
}

void ga_mat4f::set_translation(const ga_vec3f& translation)
{
	data[3][0] = translation.x;
	data[3][1] = translation.y;
	data[3][2] = translation.z;
}

ga_vec3f ga_mat4f::get_forward() const
{
	return{ data[2][0], data[2][1], data[2][2] };
}

ga_vec3f ga_mat4f::get_up() const
{
	return{ data[1][0], data[1][1], data[1][2] };
}

ga_vec3f ga_mat4f::get_right() const
{
	return{ data[0][0], data[0][1], data[0][2] };
}
