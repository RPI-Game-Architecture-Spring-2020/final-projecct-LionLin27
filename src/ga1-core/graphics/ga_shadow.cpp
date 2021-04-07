
#include "ga_shadow.h"
#include "ga_light.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

void ga_shadow::load_shader(const char* filename, std::string& contents)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::ifstream file(fullpath);

	assert(file.is_open());

	contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

ga_shadow::ga_shadow()
{
}

ga_shadow::~ga_shadow()
{
}

bool ga_shadow::init(SDL_Window* window)
{
	std::string source_vs, source_fs;
	load_shader("data/shaders/ga_shadow_vert.glsl", source_vs);
	load_shader("data/shaders/ga_shadow_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);

	_tex = new ga_texture();
	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);

	if (!_program->link())
	{
		std::cerr << "Failed to link shadow shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	setupShadowBuffers(window);

	return true;
}

void ga_shadow::readyBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, _buffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *(_tex->get_handle()), 0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);
	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);	// for reducing
	glPolygonOffset(2.0f, 4.0f);		//  shadow artifacts


	_program->use();
}

void ga_shadow::finishPass() {
	glDisable(GL_POLYGON_OFFSET_FILL);	// artifact reduction, continued

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *(_tex->get_handle()));

	glDrawBuffer(GL_FRONT);
}

void ga_shadow::bind(const ga_mat4f& view_proj, const ga_mat4f& transform)
{
	ga_uniform mvp_uniform = _program->get_uniform("shadowMVP");
	mvp_uniform.set(transform * view_proj);
}

void ga_shadow::setupShadowBuffers(SDL_Window* window)
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	_scSizeX = width;
	_scSizeY = height;

	glGenFramebuffers(1, &_buffer);

	glGenTextures(1, _tex->get_handle());
	glBindTexture(GL_TEXTURE_2D, *(_tex->get_handle()));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
		_scSizeX, _scSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// may reduce shadow border artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
