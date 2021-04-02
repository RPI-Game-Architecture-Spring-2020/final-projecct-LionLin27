#pragma once

#include "ga_program.h"
#include "ga_texture.h"

#include "math/ga_mat4f.h"
#include "math/ga_vec3f.h"

#include <string>
#include <vector>
#include <cstdint>

class ga_skybox
{
public:
	ga_skybox(ga_cube_texture* tex);
	~ga_skybox();
	bool init();
	void draw(const ga_mat4f& view, const ga_mat4f& proj);

private:
	void load_shader(const char* filename, std::string& contents);

	ga_cube_texture* _cubeTex;

	ga_shader* _vs;
	ga_shader* _fs;
	uint32_t _vao;
	uint32_t _vbos[1];
	ga_program* _program;
};
