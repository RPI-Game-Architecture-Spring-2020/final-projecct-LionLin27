#pragma once
#include "ga_obj_parser.h"
#include "ga_geometry.h"

float inline det2(float a, float b, float c, float d);

float inline det3(
	float a1, float b1, float c1,
	float a2, float b2, float c2,
	float a3, float b3, float c3);

// ADAPTED DIRECTLY FROM CUTLER'S HW3 CODE
ga_vec3f getNormal(const ga_vec3f& a, const ga_vec3f& b, const ga_vec3f& c);

float getDistanceToTri(const ga_vec3f& a, const ga_vec3f& b, const ga_vec3f& c,
	const ga_vec3f& p, const ga_vec3f& dir);

ga_vec4f getInternalDistance(const std::vector<ga_vertex>& verts, const ga_vertex& point);

void computeInteriorDistances(ga_model* model);