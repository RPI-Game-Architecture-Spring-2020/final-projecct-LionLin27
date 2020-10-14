#include "ga_procedural_model.h"
#include "ga_geometry.h"
#include "math/ga_mat4f.h"
#include "math/ga_vec2f.h"
#include "math/ga_vec3f.h"

#include <cmath>
#include <vector>

void generate_sphere(int prec, struct ga_model* model) {
	int numVertices;
	int numIndices;
	std::vector<int> indices;
	std::vector<ga_vec3f> vertices;
	std::vector<ga_vec2f> texCoords;
	std::vector<ga_vec3f> normals;
	std::vector<ga_vec3f> tangents;

	numVertices = (prec + 1) * (prec + 1);
	numIndices = prec * prec * 6;
	for (int i = 0; i < numVertices; i++) { vertices.push_back(ga_vec3f()); }
	for (int i = 0; i < numVertices; i++) { texCoords.push_back(ga_vec2f()); }
	for (int i = 0; i < numVertices; i++) { normals.push_back(ga_vec3f()); }
	for (int i = 0; i < numVertices; i++) { tangents.push_back(ga_vec3f()); }
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

	// calculate triangle vertices
	for (int i = 0; i <= prec; i++) {
		for (int j = 0; j <= prec; j++) {
			float y = (float)cos(ga_degrees_to_radians(180.0f - i * 180.0f / prec));
			float x = -(float)cos(ga_degrees_to_radians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
			float z = (float)sin(ga_degrees_to_radians(j * 360.0f / (float)(prec))) * (float)abs(cos(asin(y)));
			vertices[i * (prec + 1) + j] = { x, y, z };
			texCoords[i * (prec + 1) + j] = { ((float)j / prec), ((float)i / prec) };
			normals[i * (prec + 1) + j] = {x, y, z};

			// calculate tangent vector
			if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
				tangents[i * (prec + 1) + j] = { 0.0f, 0.0f, -1.0f };
			}
			else {
				tangents[i * (prec + 1) + j] = ga_vec3f_cross({ 0.0f, 1.0f, 0.0f }, { x, y, z });
			}
		}
	}
	// calculate triangle indices
	for (int i = 0; i < prec; i++) {
		for (int j = 0; j < prec; j++) {
			indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
			indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
			indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
		}
	}
	
	model->_vertex_format |= k_vertex_attribute_uv;
	model->_vertex_format |= k_vertex_attribute_normal;

	for (int i = 0; i < numIndices; i++) {
		ga_vertex v;

		v._position = { (vertices[indices[i]]).x ,
						(vertices[indices[i]]).y ,
						(vertices[indices[i]]).z };

		v._uv = { (texCoords[indices[i]]).x,
				  (texCoords[indices[i]]).y };

		v._normal = { (normals[indices[i]]).x ,
					  (normals[indices[i]]).y ,
					  (normals[indices[i]]).z };

		model->_vertices.push_back(v);
	}
}

