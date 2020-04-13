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
#include "math/ga_vec3f.h"

#include <cstdint>
#include <vector>

struct ga_dynamic_drawcall;

enum ga_shape_t
{
	k_shape_plane,
	k_shape_oobb,
	k_shape_count,
};

/*
** The pure virtual base shape type.
** This allows a rigid body to store an arbitrary shape with a single pointer type.
**
** Shape dimensions are always specified in local object space, as if the shape is
** being placed on an object regardless of the position of the object.
** So, for example, a sphere shape with center (0,0,0) and radius 5 will be centered
** on an object's local origin and surround it 5 units in all directions.
**
** Similarly, an axis-aligned bounding box with a min extent of (-1,-1,-1) and max
** extent of (1,1,1), the box will be sized 2x2x2 and will be centered on the
** object's local origin.
*/
struct ga_shape
{
	/*
	** Returns the type of the shape.
	*/
	virtual ga_shape_t get_type() const = 0;

	/*
	** Fills out a debug draw call with the geometry of the collision shape.
	*/
	virtual void get_debug_draw(const ga_mat4f& transform, struct ga_dynamic_drawcall* drawcall) = 0;

	/*
	** Fills out the inertia tensor matrix for the shape, given the mass.
	*/
	virtual void get_inertia_tensor(ga_mat4f& tensor, float mass) = 0;

	/*
	** Returns the vector from the center of mass to the point in space.
	*/
	virtual ga_vec3f get_offset_to_point(const ga_mat4f& transform, const ga_vec3f& point) const = 0;
};

/*
** Defines a collidable plane with a point on the plane and a plane normal.
*/
struct ga_plane final : ga_shape
{
	ga_vec3f _point;
	ga_vec3f _normal;

	ga_shape_t get_type() const override { return k_shape_plane; }
	void get_debug_draw(const ga_mat4f& transform, struct ga_dynamic_drawcall* drawcall) override;
	void get_inertia_tensor(ga_mat4f& tensor, float mass) override;
	ga_vec3f get_offset_to_point(const ga_mat4f& transform, const ga_vec3f& point) const override;
};

/*
** Defines a collidable object-oriented bounding box with a center point and
** three half-vectors that define its extents.
*/
struct ga_oobb final : ga_shape
{
	ga_vec3f _center = ga_vec3f::zero_vector();
	ga_vec3f _half_vectors[3];

	ga_shape_t get_type() const override { return k_shape_oobb; }
	void get_debug_draw(const ga_mat4f& transform, struct ga_dynamic_drawcall* drawcall) override;
	void get_inertia_tensor(ga_mat4f& tensor, float mass) override;
	ga_vec3f get_offset_to_point(const ga_mat4f& transform, const ga_vec3f& point) const override;

	void get_corners(std::vector<ga_vec3f>& corners) const;
};
