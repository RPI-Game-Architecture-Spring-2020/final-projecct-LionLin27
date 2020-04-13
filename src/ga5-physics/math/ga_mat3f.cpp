/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_mat3f.h"

#include "math/ga_math.h"

void ga_mat3f::make_identity()
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			data[i][j] = i == j ? 1.0f : 0.0f;
		}
	}
}

void ga_mat3f::make_translation(const ga_vec2f& __restrict t)
{
	make_identity();
	data[2][0] = t.x;
	data[2][1] = t.y;
	data[2][2] = 1.0f;
}

void ga_mat3f::make_scaling(float s)
{
	make_identity();
	for (int i = 0; i < 2; ++i)
	{
		data[i][i] = s;
	}
	data[2][2] = 1.0f;
}

void ga_mat3f::make_rotation_z(float angle)
{
	make_identity();

	float c = ga_cosf(angle);
	float s = ga_sinf(angle);

	data[0][0] = c;
	data[0][1] = s;
	data[1][0] = -s;
	data[1][2] = c;
	data[2][2] = 1.0f;
}

void ga_mat3f::translate(const ga_vec2f& __restrict t)
{
	ga_mat3f tmp;
	tmp.make_translation(t);
	(*this) *= tmp;
}

void ga_mat3f::scale(float s)
{
	ga_mat3f tmp;
	tmp.make_scaling(s);
	(*this) *= tmp;
}

void ga_mat3f::rotate_z(float angle)
{
	ga_mat3f tmp;
	tmp.make_rotation_z(angle);
	(*this) *= tmp;
}

ga_mat3f ga_mat3f::operator*(const ga_mat3f& __restrict b) const
{
	ga_mat3f result;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			float tmp = 0.0f;
			for (int k = 0; k < 3; ++k)
			{
				tmp += data[i][k] * b.data[k][j];
			}
			result.data[i][j] = tmp;
		}
	}
	return result;
}

ga_mat3f& ga_mat3f::operator*=(const ga_mat3f& __restrict m)
{
	(*this) = (*this) * m;
	return (*this);
}

ga_vec3f ga_mat3f::transform(const ga_vec3f& __restrict in) const
{
	ga_vec3f result;
	result.x = in.x * data[0][0] + in.y * data[1][0] + in.z * data[2][0];
	result.y = in.x * data[0][1] + in.y * data[1][1] + in.z * data[2][1];
	result.z = in.x * data[0][2] + in.y * data[1][2] + in.z * data[2][2];
	return result;
}

void ga_mat3f::transpose()
{
	ga_mat3f tmp;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tmp.data[i][j] = data[j][i];
		}
	}
	(*this) = tmp;
}

void ga_mat3f::invert()
{
	ga_mat3f tmp;
	tmp.data[0][0] = data[1][1] * data[2][2] - data[2][1] * data[1][2];
	tmp.data[1][0] = -data[0][1] * data[2][2] + data[2][1] * data[0][2];
	tmp.data[2][0] = data[0][1] * data[1][2] - data[1][1] * data[0][2];
	tmp.data[0][1] = -data[1][0] * data[2][2] + data[1][2] * data[2][0];
	tmp.data[1][1] = data[0][0] * data[2][2] - data[0][2] * data[2][0];
	tmp.data[2][1] = -data[0][0] * data[1][2] + data[0][2] * data[1][0];
	tmp.data[0][2] = data[1][0] * data[2][1] - data[1][1] * data[2][0];
	tmp.data[1][2] = -data[0][0] * data[2][1] + data[0][1] * data[2][0];
	tmp.data[2][2] = data[0][0] * data[1][1] - data[0][1] * data[1][0];

	float inv_det = data[0][0] * tmp.data[0][0] + data[1][0] * tmp.data[1][0] + data[2][0] * tmp.data[2][0];
	//VLOG_ASSERT(inv_det != 0.0f, k_vlog_error, 100, "mat3f", "Attempting to invert matrix with zero determinant.");
	inv_det = 1.0f / inv_det;

	data[0][0] = tmp.data[0][0] * inv_det;
	data[1][0] = tmp.data[0][1] * inv_det;
	data[2][0] = tmp.data[0][2] * inv_det;
	data[0][1] = tmp.data[1][0] * inv_det;
	data[1][1] = tmp.data[1][1] * inv_det;
	data[2][1] = tmp.data[1][2] * inv_det;
	data[0][2] = tmp.data[2][0] * inv_det;
	data[1][2] = tmp.data[2][1] * inv_det;
	data[2][2] = tmp.data[2][2] * inv_det;
}

bool ga_mat3f::equal(const ga_mat3f& __restrict b)
{
	bool is_not_equal = false;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			is_not_equal = is_not_equal || !ga_equalf(data[i][j], b.data[i][j]);
		}
	}
	return !is_not_equal;
}
