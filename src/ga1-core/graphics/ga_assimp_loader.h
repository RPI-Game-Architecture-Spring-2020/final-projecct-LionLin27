#pragma once
#include <vector>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/scene.h>           // Output data structure

void assimp_load_model(const char* filename, struct ga_model* model);

void assimp_load_model_force(const char* filename, struct ga_model* model);