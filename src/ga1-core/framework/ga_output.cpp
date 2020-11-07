/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_output.h"

#include "graphics/ga_shadow.h"
#include "graphics/ga_skybox.h"
#include "graphics/ga_material.h"
#include "graphics/ga_program.h"
#include "math/ga_mat4f.h"
#include "math/ga_quatf.h"

#include <cassert>
#include <iostream>
#include <SDL.h>

#include <windows.h>

#define GLEW_STATIC
#include <GL/glew.h>

// imgui
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

// TODO: move this somewhere else
ga_shadow _shadow;
ga_skybox _skybox;

ga_output::ga_output(void* win) : _window(win)
{
	int width, height;
	SDL_GetWindowSize(static_cast<SDL_Window* >(_window), &width, &height);

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW);

	_default_material = new ga_constant_color_material();
	_default_material->init();
	_wireFrame = false;

	_shadow = ga_shadow();
	_shadow.init(static_cast<SDL_Window*>(_window));

	_skybox = ga_skybox();
	_skybox.init("data/textures/cubeMap");
}

ga_output::~ga_output()
{
	delete _default_material;
}

void ga_output::update(ga_frame_params* params)
{
	//detect player input to change display mode
	if (params->_btn_down_mask & k_button_k) {
		_wireFrame = false;
	}
	if (params->_btn_down_mask & k_button_l) {
		_wireFrame = true;
	}

	// Update viewport in case window was resized:
	int width, height;
	SDL_GetWindowSize(static_cast<SDL_Window* >(_window), &width, &height);
	glViewport(0, 0, width, height);

	// Clear viewport:
	glDepthMask(GL_TRUE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// first pass
	ga_mat4f lightVmatrix, lightPmatrix;
	ga_vec3f currentLightPos = params->_lights._dirLight->_direction.scale_result(500.0f);
	//params->_lights._posLightArr[0]->_position;
	//params->_lights._dirLight->_direction.scale_result(-100.0f);

	ga_vec3f eye = currentLightPos;
	ga_vec3f at = currentLightPos - params->_lights._dirLight->_direction.scale_result(1000.0f);
	//ga_vec3f at = -currentLightPos.normal();
	ga_vec3f up = ga_vec3f::y_vector();

	lightVmatrix.make_lookat_rh(eye, at, up);
	lightPmatrix.make_perspective_rh(ga_degrees_to_radians(60.0f), (float)width / (float)height, 0.1f, 1000.0f);
		//make_orthographic(0.0f, (float)width, (float)height, 0.0f, 0.1f, 10000.0f);
		//make_perspective_rh(ga_degrees_to_radians(60.0f), (float)width / (float)height, 0.1f, 1000.0f);
	ga_mat4f lightVP = lightVmatrix * lightPmatrix;
	_shadow.readyBuffer();
	/*
	*/
	for (auto& d : params->_static_drawcalls) {
		// TODO: check if need shadow
		if (d._lit) {
			ga_mat4f mMat = d._transform;
			_shadow.bind(lightVP, mMat);

			glBindVertexArray(d._vao);

			if (d._drawBuffer) {
				glDrawArrays(d._draw_mode, 0, d._index_count);
			}
			else {
				glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
			}
		}
	}

	_shadow.finishPass();

	ga_mat4f b;
	b.make_scaling(0.5f);
	b.translate({0.5f, 0.5f, 0.5f});

	// Compute projection matrices:
	ga_mat4f perspective;
	perspective.make_perspective_rh(ga_degrees_to_radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);
	ga_mat4f view_perspective = params->_view * perspective;

	ga_mat4f ortho;
	ortho.make_orthographic(0.0f, (float)width, (float)height, 0.0f, 0.1f, 10000.0f);
	ga_mat4f view;
	view.make_lookat_rh(ga_vec3f::z_vector(), -ga_vec3f::z_vector(), ga_vec3f::y_vector());
	ga_mat4f view_ortho = view * ortho;

	ga_mat4f sky_v;
	sky_v.make_identity();
	sky_v.translate(params->_camPos.scale_result(-1));
	_skybox.draw(params->_view, perspective);

	if (_wireFrame) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Draw all static geometry:
	for (auto& d : params->_static_drawcalls)
	{
		// TODO: check if need shadow
		ga_mat4f shadowMVP2;
		if (d._lit) {
			shadowMVP2 = d._transform * lightVP * b;
			((ga_lit_material*)d._material)->bindLight(params->_view, perspective, d._transform, params->_lights, shadowMVP2);
		}
		else {
			d._material->bind(view_perspective, d._transform);
		}
		glBindVertexArray(d._vao);
		if (d._drawBuffer) {
			glDrawArrays(d._draw_mode, 0, d._index_count);
		}
		else {
			glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
		}
	}

	// Draw all dynamic geometry:
	draw_dynamic(params->_dynamic_drawcalls, view_perspective);
	draw_dynamic(params->_gui_drawcalls, view_ortho);


	//imgui
	// start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame((SDL_Window*)_window);
	ImGui::NewFrame();

	{
		// position the controls widget in the top-right corner with some margin
		ImGui::SetNextWindowPos(ImVec2(width * 5/6-5, 10), ImGuiCond_Always);
		// here we set the calculated width and also make the height to be
		// be the height of the main window also with some margin
		ImGui::SetNextWindowSize(
			ImVec2(static_cast<float>(width/6), static_cast<float>(height - 20)),
			ImGuiCond_Always
		);
		// create a window and append into it
		ImGui::Begin("Parameters", NULL, ImGuiWindowFlags_NoResize);

		ImGui::Dummy(ImVec2(0.0f, 1.0f));

		// display entity info
		float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();

		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Entity: %s", params->_selected_ent->get_name());

		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Position");
		ga_vec3f ent_pos = params->_selected_ent->get_transform().get_translation();
		float ent_pos_arr[] = { ent_pos.x, ent_pos.y, ent_pos.z };
		ImGui::InputFloat3("pos", ent_pos_arr);
		//ent_pos = { ent_pos_arr[0], ent_pos_arr[1] , ent_pos_arr[2] };
		//params->_selected_ent->set_position(ent_pos);

		float ent_trans_arr[] = { 0,0,0 };
		ImGui::SliderFloat3("trans", ent_trans_arr, -10.0f, 10.0f);
		ga_vec3f ent_trans = { ent_trans_arr[0], ent_trans_arr[1], ent_trans_arr[2] };
		params->_selected_ent->translate(ent_trans.scale_result(dt));

		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Rotation");
		float ent_rot_arr[] = { 0,0,0 };
		ImGui::SliderFloat3("rot", ent_rot_arr, -1.0f, 1.0f);
		ga_quatf axis_angle_x, axis_angle_y, axis_angle_z;
		axis_angle_x.make_axis_angle(ga_vec3f::x_vector(), ent_rot_arr[0] * dt);
		axis_angle_y.make_axis_angle(ga_vec3f::y_vector(), ent_rot_arr[1] * dt);
		axis_angle_z.make_axis_angle(ga_vec3f::z_vector(), ent_rot_arr[2] * dt);
		ga_quatf combinedRot = axis_angle_x * axis_angle_y * axis_angle_z;
		params->_selected_ent->rotate(combinedRot);



		ImGui::End();
	}

	{
		ImGui::Begin("Model Creator");
		
		if (ImGui::Button("Create Sphere")) {
			params->_herald->_create_sphere = true;
		}

		ImGui::End();
	}
	// rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());




	//**********************************

	GLenum error = glGetError();
	assert(error == GL_NONE);

	// Swap frame buffers:
	SDL_GL_SwapWindow(static_cast<SDL_Window* >(_window));
}

void ga_output::draw_dynamic(const std::vector<ga_dynamic_drawcall>& drawcalls, const ga_mat4f& view_proj)
{
	for (auto& d : drawcalls)
	{
		if (d._material)
		{
			d._material->set_color(d._color);
			d._material->bind(view_proj, d._transform);
		}
		else
		{
			_default_material->set_color(d._color);
			_default_material->bind(view_proj, d._transform);
		}

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint pos;
		glGenBuffers(1, &pos);
		glBindBuffer(GL_ARRAY_BUFFER, pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ga_vec3f) * d._positions.size(), &d._positions[0], GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		GLuint texcoord;
		if (!d._texcoords.empty())
		{
			glGenBuffers(1, &texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, texcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ga_vec2f) * d._texcoords.size(), &d._texcoords[0], GL_STREAM_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		GLuint indices;
		glGenBuffers(1, &indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * d._indices.size(), &d._indices[0], GL_STREAM_DRAW);

		glDrawElements(d._draw_mode, (GLsizei)d._indices.size(), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &indices);
		if (!d._texcoords.empty())
		{
			glDeleteBuffers(1, &texcoord);
		}
		glDeleteBuffers(1, &pos);
		glDeleteVertexArrays(1, &vao);
		glBindVertexArray(0);
	}
}
