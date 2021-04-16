#include "ga_assimp_loader.h"

#include "ga_geometry.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>

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
