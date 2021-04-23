#include "ga_assimp_loader.h"

#include "ga_geometry.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>

using namespace std;

void assimp_load_model(const char* filename, ga_model* model) {

	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	Assimp::Importer importer;

	const aiScene* _scene = importer.ReadFile(fullpath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_LimitBoneWeights |
		aiProcess_ValidateDataStructure
	);

	if (!_scene)
	{

		std::cout << importer.GetErrorString();
		_scene = importer.ReadFile(filename, 0);

		if (!_scene)
			std::cout << "error: fail to load obj\n";
	}
	else
	{
		// process the scene
		std::cout << "number of meshes: " << _scene->mNumMeshes << std::endl;

		// process the meshes
		// index hack
		int accumulated_index = 0;
		for (int i = 0; i < _scene->mNumMeshes; i++)
		{
			if (i > 0)
			{
				std::cout << "More than one mesh exists in file" << std::endl;
				break;
			}

			aiMesh* mesh = _scene->mMeshes[i];

			if (mesh->HasTextureCoords(0))
			{
				model->_vertex_format |= k_vertex_attribute_uv;
			}
			if (mesh->HasNormals())
			{
				model->_vertex_format |= k_vertex_attribute_normal;
			}
			if (mesh->HasTangentsAndBitangents())
			{
				model->_vertex_format |= k_vertex_attribute_tangent;
			}
			else {
				std::cout << "model does not have tangents" << std::endl;
			}
			if (mesh->HasVertexColors(0))
			{
				model->_vertex_format |= k_vertex_attribute_color;
			}
			if (mesh->HasBones())
			{
				//model->_vertex_format |= k_vertex_attribute_weight;
			}

			// load vertex positions 
			for (int i = 0; i < mesh->mNumVertices; i++)
			{
				ga_vertex vertex = {};
				vertex._position.x = mesh->mVertices[i].x;
				vertex._position.y = mesh->mVertices[i].y;
				vertex._position.z = mesh->mVertices[i].z;

				if (model->_vertex_format & k_vertex_attribute_uv)
				{
					vertex._uv.x = mesh->mTextureCoords[0][i].x;
					vertex._uv.y = mesh->mTextureCoords[0][i].y;
				}
				if (model->_vertex_format & k_vertex_attribute_normal)
				{
					vertex._normal.x = mesh->mNormals[i].x;
					vertex._normal.y = mesh->mNormals[i].y;
					vertex._normal.z = mesh->mNormals[i].z;
				}
				if (model->_vertex_format & k_vertex_attribute_tangent) {
					vertex._tangent.x = mesh->mTangents[i].x;
					vertex._tangent.y = mesh->mTangents[i].y;
					vertex._tangent.z = mesh->mTangents[i].z;
				}
				model->_vertices.push_back(vertex);
			}

			// load indices
			for (int f = 0; f < mesh->mNumFaces; f++)
			{
				aiFace face = mesh->mFaces[f];
				model->_indices.push_back(face.mIndices[0] + accumulated_index);
				model->_indices.push_back(face.mIndices[1] + accumulated_index);
				model->_indices.push_back(face.mIndices[2] + accumulated_index);
				if (face.mNumIndices > 3)
				{
					std::cout << "Face with more than 3 verts: mNumIndices=" << face.mNumIndices << std::endl;
				}
			}
			accumulated_index = model->_vertices.size();
		}
	}

}

// this messy function is here because I can't get assimp to properly load the bunny
// this function loads model with ga_obj_parser's method but adds tangent information with assimp
// the model will be load two times, avoid this function
void assimp_load_model_force(const char* filename, ga_model * model) {
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::vector<float> vertVals;
	std::vector<float> stVals;
	std::vector<float> normVals;

	float x, y, z;
	string content;
	ifstream fileStream(fullpath, ios::in);
	string line = "";

	if (!fileStream) {
		cout << "model file not found: " << filename << endl;
		return;
	}

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

				model->_vertices.push_back(v);
			}
		}
	}

	Assimp::Importer importer;

	const aiScene* _scene = importer.ReadFile(fullpath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		//aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_LimitBoneWeights |
		aiProcess_ValidateDataStructure
	);

	aiMesh* mesh = _scene->mMeshes[0];

	model->_vertex_format |= k_vertex_attribute_tangent;

	// load vertex positions 
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		ga_vertex& vertex = model->_vertices[i];

		vertex._tangent.x = mesh->mTangents[i].x;
		vertex._tangent.y = mesh->mTangents[i].y;
		vertex._tangent.z = mesh->mTangents[i].z;
	}
}