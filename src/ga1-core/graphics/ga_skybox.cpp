
#include "ga_skybox.h"
#include "ga_light.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <stb_image.h>

#define GLEW_STATIC
#include <GL/glew.h>

ga_skybox::ga_skybox(ga_cube_texture* tex) {
    _cubeTex = tex;
    //init();
}

ga_skybox::~ga_skybox() {}

void ga_skybox::load_shader(const char* filename, std::string& contents)
{
    extern char g_root_path[256];
    std::string fullpath = g_root_path;
    fullpath += filename;

    std::ifstream file(fullpath);

    assert(file.is_open());

    contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

bool ga_skybox::init() {

    GLfloat cubeVertexPositions[108] =
    { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
    };

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glGenBuffers(1, _vbos);

    glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    std::string source_vs, source_fs;
    load_shader("data/shaders/SKBShader_vert.glsl", source_vs);
    load_shader("data/shaders/SKBShader_frag.glsl", source_fs);

    _vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
    _fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);

    _program = new ga_program();
    _program->attach(*_vs);
    _program->attach(*_fs);

    if (!_program->link())
    {
        std::cerr << "Failed to link skybox shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
    }

    return true;
}

void ga_skybox::draw(const ga_mat4f& view, const ga_mat4f& proj) {
    _program->use();

    ga_uniform vMat = _program->get_uniform("u_vMat");
    ga_uniform pMat = _program->get_uniform("u_pMat");
    vMat.set(view);
    pMat.set(proj);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
    glEnableVertexAttribArray(0);

    glActiveTexture(GL_TEXTURE5);
    assert(glGetError() == GL_NONE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *_cubeTex->get_handle());
    assert(glGetError() == GL_NONE);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glEnable(GL_DEPTH_TEST);


}