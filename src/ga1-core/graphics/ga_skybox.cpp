
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

ga_skybox::ga_skybox() {

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

bool ga_skybox::init(const char* mapDir) {

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

    //TODO: move to ga_texture
    std::vector<std::string> mapVec;
    std::string xp = mapDir; xp = xp + "/xp.jpg"; mapVec.push_back(xp);
    std::string xn = mapDir; xn = xn + "/xn.jpg"; mapVec.push_back(xn);
    std::string yp = mapDir; yp = yp + "/yp.jpg"; mapVec.push_back(yp);
    std::string yn = mapDir; yn = yn + "/yn.jpg"; mapVec.push_back(yn);
    std::string zp = mapDir; zp = zp + "/zp.jpg"; mapVec.push_back(zp);
    std::string zn = mapDir; zn = zn + "/zn.jpg"; mapVec.push_back(zn);
    _tex_handle = loadCubemap(mapVec);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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

uint32_t ga_skybox::loadCubemap(std::vector<std::string> faces)
{
    stbi_set_flip_vertically_on_load(false);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    extern char g_root_path[256];
    std::string fullpath = g_root_path;

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        std::string path = fullpath + faces[i];
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
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
    glBindTexture(GL_TEXTURE_CUBE_MAP, _tex_handle);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glEnable(GL_DEPTH_TEST);

}