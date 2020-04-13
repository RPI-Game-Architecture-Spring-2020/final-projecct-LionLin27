/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_math.h"

#define GA_VECN_FUNCTIONS(N) \
	/* \
	** Negate the vector in place. \
	*/ \
	inline void negate() \
	{ \
		for (int i = 0; i < N; ++i) axes[i] = -axes[i]; \
	} \
	/* \
	** Return a negated version of the vector. \
	*/ \
	inline ga_vec##N##f operator-() const \
	{ \
		ga_vec##N##f result = (*this); \
		result.negate(); \
		return result; \
	} \
	/* \
	** Add the vector by another and return the result. \
	*/ \
	inline ga_vec##N##f operator+(const ga_vec##N##f& __restrict b) const \
	{ \
		ga_vec##N##f result; \
		for (int i = 0; i < N; ++i) result.axes[i] = axes[i] + b.axes[i]; \
		return result; \
	} \
	\
	/* \
	** Add the vector by another in place. \
	*/ \
	inline ga_vec##N##f& operator+=(const ga_vec##N##f& __restrict b) \
	{ \
		(*this) = (*this) + b; \
		return (*this); \
	} \
	\
	/* \
	** Subtract the vector by another and return the result. \
	*/ \
	inline ga_vec##N##f operator-(const ga_vec##N##f& __restrict b) const \
	{ \
		ga_vec##N##f result; \
		for (int i = 0; i < N; ++i) result.axes[i] = axes[i] - b.axes[i]; \
		return result; \
	} \
	\
	/* \
	** Subtract the vector by another in place. \
	*/ \
	inline ga_vec##N##f& operator-=(const ga_vec##N##f& __restrict b) \
	{ \
		(*this) = (*this) - b; \
		return (*this); \
	} \
	\
	/* \
	** Multiply the vector by another vector and return the result. \
	*/ \
	inline ga_vec##N##f operator*(const ga_vec##N##f& __restrict b) const \
	{ \
		ga_vec##N##f result; \
		for (int i = 0; i < N; ++i) result.axes[i] = axes[i] * b.axes[i]; \
		return result; \
	} \
	\
	/* \
	** Multiply the vector in place. \
	*/ \
	inline ga_vec##N##f& operator*=(const ga_vec##N##f& __restrict b) \
	{ \
		(*this) = (*this) * b; \
		return (*this); \
	} \
	\
	/* \
	** Divide the vector by another vector and return the result. \
	*/ \
	inline ga_vec##N##f operator/(const ga_vec##N##f& __restrict b) const \
	{ \
		ga_vec##N##f result; \
		for (int i = 0; i < N; ++i) result.axes[i] = axes[i] / b.axes[i]; \
		return result; \
	} \
	\
	/* \
	** Divide the vector in place. \
	*/ \
	inline ga_vec##N##f& operator/=(const ga_vec##N##f& __restrict b) \
	{ \
		(*this) = (*this) / b; \
		return (*this); \
	} \
	\
	/* \
	** Equality operator. \
	*/ \
	inline bool operator==(const ga_vec##N##f& __restrict b) \
	{ \
		return equal(b); \
	} \
	/* \
	** Scale the vector in place. \
	*/ \
	inline void scale(float s) \
	{ \
		for (int i = 0; i < N; ++i) axes[i] = axes[i] * s; \
	} \
	/* \
	** Return a vector equal to this vector scaled. \
	*/ \
	inline ga_vec##N##f scale_result(float s) const \
	{ \
		ga_vec##N##f result; \
		for (int i = 0; i < N; ++i) result.axes[i] = axes[i] * s; \
		return result; \
	} \
	\
	/* \
	** Compute the squared magnitude of the vector. \
	*/ \
	inline float mag2() const \
	{ \
		float result = 0.0f; \
		for (int i = 0; i < N; ++i) result += axes[i] * axes[i]; \
		return result; \
	} \
	\
	/* \
	** Compute the magnitude of the vector. \
	*/ \
	inline float mag() const \
	{ \
		return ga_sqrtf(mag2()); \
	} \
	\
	/* \
	** Compute the squared distance between this vector and another. \
	*/ \
	inline float dist2(const ga_vec##N##f& __restrict b) const \
	{ \
		float result = 0.0f; \
		for (int i = 0; i < N; ++i) \
		{ \
			float d = axes[i] - b.axes[i]; \
			result += d * d; \
		} \
		return result; \
	} \
	\
	/* \
	** Compute the distance between this vector and another. \
	*/ \
	inline float dist(const ga_vec##N##f& __restrict b) const \
	{ \
		return ga_sqrtf(dist2(b)); \
	} \
	\
	/* \
	** Normalize the vector in place. \
	*/ \
	inline void normalize() \
	{ \
		float m = mag(); \
		scale(1.0f / m); \
	} \
	/* \
	** Compute the normalized vector and return it. \
	*/ \
	inline ga_vec##N##f normal() const \
	{ \
		ga_vec##N##f result = (*this); \
		result.normalize(); \
		return result; \
	} \
	\
	/* \
	** Compute the dot product between this vector and another. \
	*/ \
	inline float dot(const ga_vec##N##f& __restrict b) const \
	{ \
		float result = 0.0f; \
		for (int i = 0; i < N; ++i) result += axes[i] * b.axes[i]; \
		return result; \
	} \
	/* \
	** Determine if this vector is largely equivalent to another. \
	*/ \
	inline bool equal(const ga_vec##N##f& __restrict b) const \
	{ \
		bool is_not_equal = false; \
		for (int i = 0; i < N; ++i) is_not_equal = is_not_equal || !ga_equalf(axes[i], b.axes[i]); \
		return !is_not_equal; \
	} \
	/* \
	** Project this vector onto another and return the result. \
	*/ \
	inline ga_vec##N##f project_onto(const ga_vec##N##f& __restrict b) const \
	{ \
		ga_vec##N##f b_norm = b.normal(); \
		ga_vec##N##f result = b_norm.scale_result(dot(b_norm)); \
		return result; \
	} \
	/* \
	** Absolute value projection. \
	*/ \
	inline ga_vec##N##f project_onto_abs(const ga_vec##N##f& __restrict b) const \
	{ \
		ga_vec##N##f b_norm = b.normal(); \
		ga_vec##N##f result = b_norm.scale_result(ga_absf(dot(b_norm))); \
		return result; \
	}
