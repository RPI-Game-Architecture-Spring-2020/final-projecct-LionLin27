#pragma once

#include <SDL.h>
#include "ga_program.h"
#include "ga_texture.h"
#include "ga_light.h"

#include "math/ga_mat4f.h"
#include "math/ga_vec3f.h"

#include <string>

class ga_shadow
{
public:
	ga_shadow();
	~ga_shadow();
	bool init(SDL_Window* window);
	void readyBuffer();
	void finishPass();
	void bind(const ga_mat4f& view_proj, const ga_mat4f& transform);

private:
	void setupShadowBuffers(SDL_Window* window);
	void load_shader(const char* filename, std::string& contents);

	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_texture* _tex;
	uint32_t _buffer;
	int _scSizeX, _scSizeY;
};
