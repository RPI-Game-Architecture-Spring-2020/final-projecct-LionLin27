#include "ga_procedural_model.h"
#include "ga_geometry.h"
#include "math/ga_mat4f.h"
#include "math/ga_vec2f.h"
#include "math/ga_vec3f.h"

#include <cmath>
#include <vector>

#include <cassert>

void generate_arrow(struct ga_model* model) {
	int numVertices;
	int numIndices;
	std::vector<int> indices;
	std::vector<ga_vec3f> vertices;

	numVertices = 12;
	vertices.push_back({ -1,0,0 });
	vertices.push_back({ 0,0,-4 });
	vertices.push_back({ 1,0,0 });
	vertices.push_back({ 0,0,-4 });
	vertices.push_back({ -1,0,0 });
	vertices.push_back({ 1,0,0 });
	vertices.push_back({ 0,-1,0 });
	vertices.push_back({ 0,0,-4 });
	vertices.push_back({ 0,1,0 });
	vertices.push_back({ 0,0,-4 });
	vertices.push_back({ 0,-1,0 });
	vertices.push_back({ 0,1,0 });

	for (int i = 0; i < numVertices; i++) {
		ga_vertex v;

		v._position = vertices[i];

		model->_vertices.push_back(v);
	}
}

void generate_plane(struct ga_model* model) {
	int numVertices;
	int numIndices;
	std::vector<int> indices;
	std::vector<ga_vec3f> vertices;
	std::vector<ga_vec2f> texCoords;
	std::vector<ga_vec3f> normals;
	std::vector<ga_vec3f> tangents;

	numVertices = 6;
	vertices.push_back({ -1.0, 0.0,  1.0 });
	vertices.push_back({ 1.0,  0.0,  1.0 });
	vertices.push_back({ 1.0,  0.0, -1.0 });
	vertices.push_back({ 1.0,  0.0, -1.0 });
	vertices.push_back({ -1.0, 0.0, -1.0 });
	vertices.push_back({ -1.0, 0.0,  1.0 });
	normals.push_back({ 0,1,0 });
	normals.push_back({ 0,1,0 });
	normals.push_back({ 0,1,0 });
	normals.push_back({ 0,1,0 });
	normals.push_back({ 0,1,0 });
	normals.push_back({ 0,1,0 });
	texCoords.push_back({ 0.0,0.0 });
	texCoords.push_back({ 1.0,0.0 });
	texCoords.push_back({ 1.0,1.0 }); 
	texCoords.push_back({ 1.0,1.0 });
	texCoords.push_back({ 0.0,1.0 });
	texCoords.push_back({ 0.0,0.0 });

	model->_vertex_format |= k_vertex_attribute_uv;
	model->_vertex_format |= k_vertex_attribute_normal;

	for (int i = 0; i < numVertices; i++) {
		ga_vertex v;

		v._position = vertices[i];

		v._uv = texCoords[i];

		v._normal = normals[i];

		model->_vertices.push_back(v);
	}
}

void generate_cube(struct ga_model* model) {
	static ga_vec3f vertices[] = {
		// Front
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// Top
		-1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
		// Back
		 1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		-1.0,  1.0, -1.0,
		 1.0,  1.0, -1.0,
		 // Bottom
		 -1.0, -1.0, -1.0,
		  1.0, -1.0, -1.0,
		  1.0, -1.0,  1.0,
		 -1.0, -1.0,  1.0,
		 // Left
		 -1.0, -1.0, -1.0,
		 -1.0, -1.0,  1.0,
		 -1.0,  1.0,  1.0,
		 -1.0,  1.0, -1.0,
		 // Right
		  1.0, -1.0,  1.0,
		  1.0, -1.0, -1.0,
		  1.0,  1.0, -1.0,
		  1.0,  1.0,  1.0,
	};
	static ga_vec2f texcoords[] = {
		// Front
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		// Top
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		// Back
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		// Bottom
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		// Left
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		// Right
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};
	static short indices[] = {
		// Front
		0,  1,  2,
		2,  3,  0,
		// Top
		4,  5,  6,
		6,  7,  4,
		// Back
		8,  9, 10,
		10, 11,  8,
		// Bottom
		12, 13, 14,
		14, 15, 12,
		// Left
		16, 17, 18,
		18, 19, 16,
		// Right
		20, 21, 22,
		22, 23, 20,
	};

	int numVertices = uint32_t(sizeof(vertices) / sizeof(*vertices));
	int numIndices = uint32_t(sizeof(indices) / sizeof(*indices));

	assert(numVertices == 24);
	assert(numIndices == 36);

	model->_vertex_format |= k_vertex_attribute_uv;
	model->_vertex_format |= k_vertex_attribute_normal;
	model->_vertex_format |= k_vertex_attribute_tangent;

	for (int i = 0; i < numIndices; i++) {
		ga_vertex v;
		int index = indices[i];
		v._position = vertices[index];
		v._uv = texcoords[index];

		if (i / 6 == 0) {
			//front
			v._normal = { 0,0,1 };
			v._tangent = { 1,0,0 };
		}
		else if (i / 6 == 1) {
			//top
			v._normal = { 0,1,0 };
			v._tangent = { 0,0,1 };
		}
		else if (1 / 6 == 2) {
			v._normal = { 0,0,-1 };
			v._tangent = { -1,0,0 };
		}
		else if (i / 6 == 3) {
			v._normal = { 0,-1,0 };
			v._tangent = { 0,0,-1 };
		}
		else if (i / 6 == 4) {
			v._normal = { -1,0,0 };
			v._tangent = { 0,0,1 };
		}
		else if (i / 6 == 5) {
			v._normal = { 1,0,0 };
			v._tangent = { 0,0,-1 };
		}

		model->_vertices.push_back(v);
	}
}

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
	model->_vertex_format |= k_vertex_attribute_tangent;

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

		v._tangent = { (tangents[indices[i]]).x ,
					   (tangents[indices[i]]).y ,
					   (tangents[indices[i]]).z };
					   
		model->_vertices.push_back(v);
	}
}

void generate_torus(float inner, float outer, int prec, struct ga_model* model)
{
	int numVertices;
	int numIndices;

	std::vector<int> indices;
	std::vector<ga_vec3f> vertices;
	std::vector<ga_vec2f> texCoords;
	std::vector<ga_vec3f> normals;
	std::vector<ga_vec3f> sTangents;
	std::vector<ga_vec3f> tTangents;
	
	numVertices = (prec + 1) * (prec + 1);
	numIndices =  prec* prec * 6;
	for (int i = 0; i < numVertices; i++) { vertices.push_back(ga_vec3f()); }
	for (int i = 0; i < numVertices; i++) { texCoords.push_back(ga_vec2f()); }
	for (int i = 0; i < numVertices; i++) { normals.push_back(ga_vec3f()); }
	for (int i = 0; i < numVertices; i++) { sTangents.push_back(ga_vec3f()); }
	for (int i = 0; i < numVertices; i++) { tTangents.push_back(ga_vec3f()); }
	for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

	// calculate first ring
	for (int i = 0; i < prec + 1; i++) {

		float amt = ga_degrees_to_radians(i * 360.0f / prec);

		ga_quatf axis_angle;
		axis_angle.make_axis_angle(ga_vec3f::z_vector(), amt);
		ga_mat4f rMat;
		rMat.make_identity();
		rMat.make_rotation(axis_angle);

		ga_vec4f initPos4(rMat.transform({ outer, 0.0f, 0.0f, 1.0f }));
		ga_vec3f initPos = { initPos4.x, initPos4.y, initPos4.z };
		initPos.x += inner;
		vertices[i] = initPos;
		texCoords[i] = { 0.0f, ((float)i / (float)prec) };

		rMat.make_identity();
		rMat.make_rotation(axis_angle);
		ga_vec4f temp = rMat.transform({ 0.0f, -1.0f, 0.0f, 1.0f });
		tTangents[i] = {temp.x, temp.y, temp.z};

		sTangents[i] = { 0.0f, 0.0f, -1.0f };
		normals[i] = ga_vec3f_cross(tTangents[i], sTangents[i]);
	}
	// rotate the first ring about Y to get the other rings
	for (int ring = 1; ring < prec + 1; ring++) {
		for (int i = 0; i < prec + 1; i++) {
			float amt = (float)ga_degrees_to_radians((float)ring * 360.0f / (prec));

			ga_quatf axis_angle;
			axis_angle.make_axis_angle(ga_vec3f::y_vector(), amt);
			ga_mat4f rMat;
			rMat.make_identity();
			rMat.make_rotation(axis_angle);
			ga_vec4f v4 = rMat.transform({ vertices[i].x, vertices[i].y, vertices[i].z, 1.0f });
			vertices[ring * (prec + 1) + i] = {v4.x, v4.y, v4.z};

			texCoords[ring * (prec + 1) + i] = { (float)ring * 2.0f / (float)prec, texCoords[i].y };
			//if (texCoords[ring * (prec + 1) + i].x > 1.0) texCoords[ring * (prec + 1) + i].x -= 1.0f;

			rMat.make_rotation(axis_angle);
			v4 = { sTangents[i].x, sTangents[i].y, sTangents[i].z, 1.0f};
			v4 = rMat.transform(v4);
			sTangents[ring * (prec + 1) + i] = {v4.x, v4.y, v4.z};

			v4 = { tTangents[i].x, tTangents[i].y, tTangents[i].z, 1.0f };
			v4 = rMat.transform(v4);
			tTangents[ring * (prec + 1) + i] = { v4.x, v4.y, v4.z };

			v4 = { normals[i].x, normals[i].y, normals[i].z, 1.0f };
			v4 = rMat.transform(v4);
			normals[ring * (prec + 1) + i] = { v4.x, v4.y, v4.z };
		}
	}
	// calculate triangle indices
	for (int ring = 0; ring < prec; ring++) {
		for (int i = 0; i < prec; i++) {
			indices[((ring * prec + i) * 2) * 3 + 0] = ring * (prec + 1) + i;
			indices[((ring * prec + i) * 2) * 3 + 1] = (ring + 1) * (prec + 1) + i;
			indices[((ring * prec + i) * 2) * 3 + 2] = ring * (prec + 1) + i + 1;
			indices[((ring * prec + i) * 2 + 1) * 3 + 0] = ring * (prec + 1) + i + 1;
			indices[((ring * prec + i) * 2 + 1) * 3 + 1] = (ring + 1) * (prec + 1) + i;
			indices[((ring * prec + i) * 2 + 1) * 3 + 2] = (ring + 1) * (prec + 1) + i + 1;
		}
	}

	model->_vertex_format |= k_vertex_attribute_uv;
	model->_vertex_format |= k_vertex_attribute_normal;
	model->_vertex_format |= k_vertex_attribute_tangent;

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

		v._tangent = {  (tTangents[indices[i]]).x,
						(tTangents[indices[i]]).y,
						(tTangents[indices[i]]).z };

		model->_vertices.push_back(v);
	}
}

