#include "ga_obj_parser.h"

#include "ga_geometry.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>

using namespace std;

// TODO : put this somewhere else
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

float getDistanceToTri(const ga_vec3f & a, const ga_vec3f & b, const ga_vec3f & c,
	const ga_vec3f & p, const ga_vec3f & dir) {

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
		if (fabs(detA) <= 0.000001) return 0;
		assert(fabs(detA) >= 0.000001);

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

float getInternalDistance(const std::vector<ga_vertex>& verts, const ga_vertex & point) {
	ga_vec3f n = -point._normal;
	ga_vec3f p = point._position;
	float bestDistance = INFINITY;

	assert((verts.size() % 3) == 0); 
	for (size_t v = 0; v < verts.size(); v += 3) {
		bestDistance = std::min(
			getDistanceToTri(verts[v]._position, verts[v+1]._position, verts[v+2]._position, p, n),
			bestDistance);
	}
	return bestDistance;
}

void obj_to_model(const char* filename, ga_model* model)
{
	std::vector<float> vertVals;
	std::vector<float> stVals;
	std::vector<float> normVals;


	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	float x, y, z;
	string content;
	ifstream fileStream(fullpath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0) {
			stringstream ss(line.erase(0, 1));
			ss >> x; ss >> y; ss >> z;
			vertVals.push_back(x);
			vertVals.push_back(y);
			vertVals.push_back(z);
		}
		if (line.compare(0, 2, "vt") == 0) {
			model->_vertex_format |= k_vertex_attribute_uv;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			stVals.push_back(x);
			stVals.push_back(y);
		}
		if (line.compare(0, 2, "vn") == 0) {
			model->_vertex_format |= k_vertex_attribute_normal;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			normVals.push_back(x);
			normVals.push_back(y);
			normVals.push_back(z);
		}
		if (line.compare(0, 2, "f ") == 0) {
			string oneCorner, v, t, n;
			stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++) {
				getline(ss, oneCorner, ' ');
				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');

				int vertRef = (stoi(v) - 1) * 3;
				int tcRef = (stoi(t) - 1) * 2;
				int normRef = (stoi(n) - 1) * 3;

				ga_vertex v;

				v._position.x = vertVals[vertRef];
				v._position.y = vertVals[vertRef + 1];
				v._position.z = vertVals[vertRef + 2];

				v._uv.x = stVals[tcRef];
				v._uv.y = stVals[tcRef + 1];

				v._normal.x = normVals[normRef];
				v._normal.y = normVals[normRef + 1];
				v._normal.z = normVals[normRef + 2];

				v._internal_distance_n = -1;

				model->_vertices.push_back(v);
			}
		}
		// TODO : Jay addition for refraction
		if (line.find("for refraction") != std::string::npos) {
			// mark for internal distance calculation
			model->_vertex_format |= k_vertex_attribute_internal_dist;
		}
	}
	// if model is marked for internal distance calculation
	if (model->_vertex_format & k_vertex_attribute_internal_dist) {
		std::cout << "DEBUG: Estimating internal distances for model." << std::endl;
		for (int v = 0; v < model->_vertices.size(); ++v) {
			model->_vertices[v]._internal_distance_n
				= getInternalDistance(model->_vertices, model->_vertices[v]);
		}
	}
}
