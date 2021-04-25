#include <iostream>

#include "ga_interiorDistanceScanner.h"

#include "ga_obj_parser.h"
#include "ga_geometry.h"

float inline det2(float a, float b, float c, float d) {
	return a * d - b * c;
}

float inline det3(
	float a1, float b1, float c1,
	float a2, float b2, float c2,
	float a3, float b3, float c3) {
	return
		a1 * det2(b2, b3, c2, c3)
		- b1 * det2(a2, a3, c2, c3)
		+ c1 * det2(a2, a3, b2, b3);
}

// ADAPTED DIRECTLY FROM CUTLER'S HW3 CODE
ga_vec3f getNormal(const ga_vec3f& a, const ga_vec3f& b, const ga_vec3f& c) {
	ga_vec3f v12 = b;
	v12 -= a;
	ga_vec3f v23 = c;
	v23 -= b;
	ga_vec3f normal = ga_vec3f_cross(v12, v23);
	normal.normalize();
	return normal;
}

float getDistanceToTri(const ga_vec3f& a, const ga_vec3f& b, const ga_vec3f& c,
	const ga_vec3f& p, const ga_vec3f& dir) {

	ga_vec3f normal = getNormal(a, b, c);
	float d = normal.dot(a);

	float numer = d - p.dot(normal);
	float denom = dir.dot(normal);

	if (denom == 0) return INFINITY;  // parallel to plane

	float t = numer / denom;
	if (t > 0.0001) {
		// we strike plane, are we inside tri?
		float detA = det3(
			a.x - b.x, a.x - c.x, dir.x,
			a.y - b.y, a.y - c.y, dir.y,
			a.z - b.z, a.z - c.z, dir.z);
		if (fabs(detA) <= 0.000001) return INFINITY;

		float beta = det3(
			a.x - p.x, a.x - c.x, dir.x,
			a.y - p.y, a.y - c.y, dir.y,
			a.z - p.z, a.z - c.z, dir.z) / detA;
		float gamma = det3(
			a.x - b.x, a.x - p.x, dir.x,
			a.y - b.y, a.y - p.y, dir.y,
			a.z - b.z, a.z - p.z, dir.z) / detA;

		if (beta >= -0.00001 && beta <= 1.00001 &&
			gamma >= -0.00001 && gamma <= 1.00001 &&
			beta + gamma <= 1.00001) {
			return t;
		}
	}
	return INFINITY;
}

ga_vec4f getInternalDistance(const std::vector<ga_vertex>& verts, const ga_vertex& point) {
	ga_vec3f n = -point._normal;
	ga_vec3f p = point._position;
	float bestDistance = INFINITY;

	for (size_t v = 0; v < verts.size(); v += 3) {
		bestDistance = std::min(
			getDistanceToTri(verts[v]._position, verts[v + 1]._position, verts[v + 2]._position, p, n),
			bestDistance);
	}
	ga_vec4f bestLine;
	bestLine.x = n.x * bestDistance;
	bestLine.y = n.y * bestDistance;
	bestLine.z = n.z * bestDistance;
	bestLine.w = bestDistance;
	return bestLine;
}


void computeInteriorDistances(ga_model* model) {
	model->_vertex_format |= k_vertex_attribute_internal_dist;
	for (int v = 0; v < model->_vertices.size(); ++v) {
		model->_vertices[v].internal_dist
			= getInternalDistance(model->_vertices, model->_vertices[v]);
		if (v != 0 && v % (model->_vertices.size() / 10) == 0)
			std::cout << "Finished " << v << " out of " << model->_vertices.size() << std::endl;
	}
}