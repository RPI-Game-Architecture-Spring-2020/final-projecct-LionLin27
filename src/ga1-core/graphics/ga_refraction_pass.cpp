
#include "ga_refraction_pass.h"
#include "ga_material.h"
#include "ga_light.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <graphics/ga_material.h>


void ga_refraction_pass::load_shader(const char* filename, std::string& contents)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::ifstream file(fullpath);

	assert(file.is_open());

	contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

ga_refraction_pass::ga_refraction_pass()
{
}

ga_refraction_pass::~ga_refraction_pass()
{
}

bool ga_refraction_pass::init(SDL_Window* window)
{
	std::string source_vs, source_fs;
	load_shader("data/shaders/ga_refraction_pass_vert.glsl", source_vs);
	load_shader("data/shaders/ga_refraction_pass_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);

	if (!_fs->compile())
	{
		std::cerr << "Failed to compile refraction pass fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile refraction pass vertex shader:\n\t" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_depthTex = new ga_texture();
	_normalTex = new ga_texture();
	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);

	if (!_program->link())
	{
		std::cerr << "Failed to link refraction pass shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	setupRefractionBuffers(window);

	return true;
}

void ga_refraction_pass::readyBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, _Buffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *(_depthTex->get_handle()), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *(_normalTex->get_handle()), 0);

	glClearColor(0, 1, 1, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);	// for reducing
	glPolygonOffset(2.0f, 4.0f);		//  shadow artifacts

	_program->use();
}

void ga_refraction_pass::finishPass() {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
	//glDrawBuffer(GL_FRONT);
	glDisable(GL_POLYGON_OFFSET_FILL);	// artifact reduction, continued
	//glDrawBuffer(GL_FRONT);
}

void ga_refraction_pass::bindTextures(int depthTexIndex, int normalTexIndex) {
	glActiveTexture(GL_TEXTURE0 + depthTexIndex);
	glBindTexture(GL_TEXTURE_2D, *(_depthTex->get_handle()));
	glActiveTexture(GL_TEXTURE0 + normalTexIndex);
	glBindTexture(GL_TEXTURE_2D, *(_normalTex->get_handle()));
}

void ga_refraction_pass::bind(const ga_mat4f& view, const ga_mat4f& proj, 
	const ga_mat4f& transform, const ga_vec3f& eyePos, ga_refractive_lit_material * mat)
{
	/*
CC-uniform sampler2D u_normMap;
CC-uniform mat4 u_mvMat;
CC-uniform mat4 u_vMat;
CC-uniform bool b_useNormalMap;
CC-uniform float f_normalStr;
CC-uniform mat4 u_mvp;
CC-uniform mat4 u_mvMat;
	*/
	_program->use();

	ga_uniform vMat = _program->get_uniform("u_vMat");
	vMat.set(view);

	ga_uniform mvMat = _program->get_uniform("u_mvMat");
	ga_uniform mvp_uniform = _program->get_uniform("u_mvp");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");
	mvMat.set(transform * view);
	mvp_uniform.set(transform * view * proj);

	ga_uniform eyePos_uniform = _program->get_uniform("v_eyePos");
	eyePos_uniform.set(eyePos);

	ga_uniform useNormalMap = _program->get_uniform("b_useNormalMap");
	ga_uniform normalmap_uniform = _program->get_uniform("u_normMap");

	useNormalMap.set(mat->get_useNormalMap());
	if (mat->get_useNormalMap()) {
		normalmap_uniform.set(*(mat->get_normalMap()), 2);
	}
	ga_uniform normalStr_uniform = _program->get_uniform("f_normalStr");
	normalStr_uniform.set(mat->get_normalStr());

}

void ga_refraction_pass::setupRefractionBuffers(SDL_Window* window)
{
	int width, height;
	float scaling = 1.0;
	SDL_GetWindowSize(window, &width, &height);
	_scSizeX = width * scaling;
	_scSizeY = height * scaling;

	glGenFramebuffers(1, &_Buffer);


	glGenTextures(1, _depthTex->get_handle());
	glBindTexture(GL_TEXTURE_2D, *(_depthTex->get_handle()));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
		_scSizeX, _scSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 4-color normal texture and depth
	glGenTextures(1, _normalTex->get_handle());
	glBindTexture(GL_TEXTURE_2D, *(_normalTex->get_handle()));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
		_scSizeX, _scSizeY, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}
