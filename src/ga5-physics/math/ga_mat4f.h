#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_quatf.h"
#include "math/ga_vec3f.h"
#include "math/ga_vec4f.h"

/*
** Floating point 4x4 matrix.
*/
struct ga_mat4f
{
	float data[4][4];

	/*
	** Build an identity matrix.
	*/
	void make_identity();

	/*
	** Build a translation matrix.
	*/
	void make_translation(const ga_vec3f& __restrict t);

	/*
	** Build a uniform scaling matrix.
	*/
	void make_scaling(float s);

	/*
	** Build a rotation matrix.
	*/
	void make_rotation(const ga_quatf& __restrict q);

	/*
	** Apply translation to the given matrix.
	*/
	void translate(const ga_vec3f& __restrict t);

	/*
	** Apply uniform scaling to the given matrix.
	*/
	void scale(float s);

	/*
	** Apply rotation to the given matrix.
	*/
	void rotate(const ga_quatf& __restrict q);

	/*
	** Multiply two matrices and store the result in a third.
	*/
	ga_mat4f operator*(const ga_mat4f& __restrict b) const;

	/*
	** Multiply a matrix by another, storing the result in the first.
	*/
	ga_mat4f& operator*=(const ga_mat4f& __restrict m);

	/*
	** Transform a vector by a matrix.
	*/
	ga_vec4f transform(const ga_vec4f& __restrict in) const;

	/*
	** Transforms a vector by a matrix.
	** 
	** This method is similar to transform_point but it ignores 
	** translation as translating a vector is nonsensical.
	*/
	ga_vec3f transform_vector(const ga_vec3f& __restrict in) const;

	/*
	** Transforms a point by a matrix.
	*/
	ga_vec3f transform_point(const ga_vec3f& __restrict in) const;

	/*
	** Transpose a matrix.
	*/
	void transpose();

	/*
	** Invert the given matrix.
	*/
	void invert();

	/*
	** Return the inverse of this matrix.
	*/
	ga_mat4f inverse() const;

	/*
	** Build a orthographic projection matrix.
	*/
	void make_orthographic(float left, float right, float bottom, float top, float z_near, float z_far);

	/*
	** Build a right-handed perspective projection matrix.
	*/
	void make_perspective_rh(float angle, float aspect, float z_near, float z_far);

	/*
	** Build a right-handed model-view matrix.
	*/
	void make_lookat_rh(const ga_vec3f& __restrict eye, const ga_vec3f& __restrict at, const ga_vec3f& __restrict up);

	/*
	** Determine if two matrices are largely equivalent.
	*/
	bool equal(const ga_mat4f& __restrict b);

	/*
	** Get the translation portion of the matrix.
	**
	** The fourth column of the matrix.
	*/
	ga_vec3f get_translation() const;

	/*
	** Set the translation portion of the matrix.
	**
	** The fourth column of the matrix.
	*/
	void set_translation(const ga_vec3f& translation);

	/*
	** Get the forward vector from the matrix.
	**
	** The first column of the matrix.
	*/
	ga_vec3f get_forward() const;

	/*
	** Get the up vector from the matrix.
	**
	** The second column of the matrix.
	*/
	ga_vec3f get_up() const;

	/*
	** Get the right vector from the matrix.
	**
	** The third column of the matrix.
	*/
	ga_vec3f get_right() const;
};
