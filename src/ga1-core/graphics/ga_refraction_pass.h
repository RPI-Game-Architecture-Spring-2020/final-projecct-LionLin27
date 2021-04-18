#pragma once

#include <SDL.h>
#include "ga_material.h"
#include "ga_program.h"
#include "ga_texture.h"
#include "ga_light.h"

#include "math/ga_mat4f.h"
#include "math/ga_vec3f.h"

#include <string>

class ga_refraction_pass
{
public:
	ga_refraction_pass();
	~ga_refraction_pass();
	bool init(SDL_Window* window);
	void readyBuffer();
	void finishPass();
	void bindTextures(int depthTexIndex, int normalTexIndex);
	void bind(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform, const ga_vec3f& eyePos, ga_refractive_lit_material* mat);
private:
	void setupRefractionBuffers(SDL_Window* window);
	void load_shader(const char* filename, std::string& contents);

	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_texture* _depthTex;
	uint32_t _Buffer;
	ga_texture* _normalTex;
	uint32_t _normalBuffer;
	int _scSizeX, _scSizeY;
};
