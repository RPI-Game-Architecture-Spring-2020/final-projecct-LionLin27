/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_egg_parser.h"

#include "ga_animation.h"
#include "ga_geometry.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>

void parse_texture_data(std::ifstream &file, struct ga_model* model);
void parse_vertex_data(std::ifstream &file, struct ga_model* model);
void parse_poly_data(std::ifstream &file, struct ga_model* model);
void parse_joint_data(std::ifstream &file, struct ga_model* model, uint32_t depth = 0);
void parse_joint_anim_data(std::ifstream &file, struct ga_animation* animation, uint32_t depth = 0);

void egg_to_model(const char* filename, ga_model* model)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::ifstream file(fullpath);

	char data[128];
	file >> data;
	while (!file.eof())
	{
		if (strcmp(data, "<Vertex>") == 0)
		{
			parse_vertex_data(file, model);
		}
		else if (strcmp(data, "<Polygon>") == 0)
		{
			parse_poly_data(file, model);
		}
		else if (strcmp(data, "<Joint>") == 0)
		{
			model->_vertex_format |= k_vertex_attribute_weight;
			model->_skeleton = new ga_skeleton();
			parse_joint_data(file, model);
		}
		else if (strcmp(data, "<Texture>") == 0)
		{
			parse_texture_data(file, model);
		}

		file >> data;
	}
}

void parse_texture_data(std::ifstream &file, ga_model* model)
{
	char data[128];
	file >> data;
	while (strcmp(data, "{") != 0)
	{
		file >> data;
	}
	int open_parens = 1;

	file >> data;
	model->_texture_name = strtok(data, "\"");

	while (open_parens > 0)
	{
		file >> data;
		if (strcmp(data, "{") == 0)
		{
			open_parens += 1;
		}
		if (strcmp(data, "}") == 0)
		{
			open_parens -= 1;
		}
	}
}

void parse_vertex_data(std::ifstream &file, ga_model* model)
{
	ga_vertex v;

	char data[128];
	file >> data;
	while (strcmp(data, "{") != 0)
	{
		file >> data;
	}
	int open_parens = 1;

	file >> data; v._position.x = (float)atof(data);
	file >> data; v._position.y = (float)atof(data);
	file >> data; v._position.z = (float)atof(data);

	while (open_parens > 0)
	{
		file >> data;
		if (strcmp(data, "<Normal>") == 0)
		{
			model->_vertex_format |= k_vertex_attribute_normal;

			file >> data; open_parens += 1;
			file >> data; v._normal.x = (float)atof(data);
			file >> data; v._normal.y = (float)atof(data);
			file >> data; v._normal.z = (float)atof(data);
			file >> data; open_parens -= 1;
		}
		else if (strcmp(data, "<UV>") == 0)
		{
			model->_vertex_format |= k_vertex_attribute_uv;

			file >> data; open_parens += 1;
			file >> data; v._uv.x = (float)atof(data);
			file >> data; v._uv.y = (float)atof(data);
			file >> data; open_parens -= 1;
		}
		else if (strcmp(data, "<RGBA>") == 0)
		{
			model->_vertex_format |= k_vertex_attribute_color;

			file >> data; open_parens += 1;
			file >> data; v._color.x = (float)atof(data);
			file >> data; v._color.y = (float)atof(data);
			file >> data; v._color.z = (float)atof(data);
			file >> data; open_parens -= 1;
		}
		else if (strcmp(data, "{") == 0)
		{
			open_parens += 1;
		}
		else if (strcmp(data, "}") == 0)
		{
			open_parens -= 1;
		}
	}

	model->_vertices.push_back(v);
}

void parse_poly_data(std::ifstream &file, ga_model* model)
{
	int vertex_count = 0;
	uint32_t indices[4];

	char data[128];
	file >> data;
	while (strcmp(data, "{") != 0)
	{
		file >> data;
	}
	int open_parens = 1;

	while(open_parens > 0)
	{
		file >> data;
		if (strcmp(data, "<VertexRef>") == 0)
		{
			file >> data; open_parens += 1;
			file >> data;
			while (strcmp(data, "<Ref>") != 0)
			{
				assert(vertex_count < 4);
				indices[vertex_count] = atoi(data);
				file >> data; 
				vertex_count++;
			}
		}
		else if (strcmp(data, "{") == 0)
		{
			open_parens += 1;
		}
		else if (strcmp(data, "}") == 0)
		{
			open_parens -= 1;
		}
	}

	model->_indices.push_back(indices[0]);
	model->_indices.push_back(indices[1]);
	model->_indices.push_back(indices[2]);

	if (vertex_count > 3)
	{
		model->_indices.push_back(indices[0]);
		model->_indices.push_back(indices[2]);
		model->_indices.push_back(indices[3]);
	}
}

void parse_joint_data(std::ifstream &file, ga_model* model, uint32_t depth)
{
	ga_joint* j = new ga_joint;
	j->_parent = depth > 0 ? depth - 1 : INT_MAX;
	// Push the joint now.
	model->_skeleton->_joints.push_back(j);

	ga_mat4f local_matrix;

	char data[128];
	// Skip over the name.
	file >> data;
	while (strcmp(data, "{") != 0)
	{
		file >> data;
	}
	int open_parens = 1;

	while(open_parens > 0)
	{
		file >> data;
		if (strcmp(data, "<Transform>") == 0)
		{
			file >> data; open_parens += 1;
			file >> data;
			file >> data; open_parens += 1;
			file >> data; local_matrix.data[0][0] = (float)atof(data);
			file >> data; local_matrix.data[0][1] = (float)atof(data);
			file >> data; local_matrix.data[0][2] = (float)atof(data);
			file >> data; local_matrix.data[0][3] = (float)atof(data);
			file >> data; local_matrix.data[1][0] = (float)atof(data);
			file >> data; local_matrix.data[1][1] = (float)atof(data);
			file >> data; local_matrix.data[1][2] = (float)atof(data);
			file >> data; local_matrix.data[1][3] = (float)atof(data);
			file >> data; local_matrix.data[2][0] = (float)atof(data);
			file >> data; local_matrix.data[2][1] = (float)atof(data);
			file >> data; local_matrix.data[2][2] = (float)atof(data);
			file >> data; local_matrix.data[2][3] = (float)atof(data);
			file >> data; local_matrix.data[3][0] = (float)atof(data);
			file >> data; local_matrix.data[3][1] = (float)atof(data);
			file >> data; local_matrix.data[3][2] = (float)atof(data);
			file >> data; local_matrix.data[3][3] = (float)atof(data);

			// Calculate the bind matrix by using the parent's.
			ga_mat4f parent_matrix;
			parent_matrix.make_identity();
			if (depth > 0)
			{
				parent_matrix = model->_skeleton->_joints[depth - 1]->_world;
			}
			j->_world = local_matrix * parent_matrix;

			file >> data; open_parens -= 1;
			file >> data; open_parens -= 1;
		}
		else if (strcmp(data, "<VertexRef>") == 0)
		{
			file >> data; open_parens += 1;
			file >> data;

			std::vector<uint32_t> vertices;
			while(strcmp(data,"<Scalar>") != 0)
			{
				int vertex_index = atoi(data);
				// TODO: The panda requires i - 1.
				vertices.push_back(vertex_index);
				file >> data;
			}

			// Read in "membership."
			file >> data;
			file >> data; open_parens += 1;
			file >> data;
			float influence = (float)atof(data);
			file >> data; open_parens -= 1;

			// Add the joint and weight to the vertices it influences.
			for (int i = 0; i < vertices.size(); i++)
			{
				ga_vertex* vertex = &model->_vertices[vertices[i]];
				uint32_t joint_index = 0;
				while (vertex->_weights[joint_index] > 0.0f) joint_index++;
				assert(joint_index < ga_vertex::k_max_joint_weights);
				vertex->_joints[joint_index] = depth;
				vertex->_weights[joint_index] = influence;
			}
		}
		else if (strcmp(data, "<Joint>") == 0)
		{
			parse_joint_data(file, model, depth + 1);
		}
		else if (strcmp(data, "{") == 0)
		{
			open_parens += 1;
		}
		else if (strcmp(data, "}") == 0)
		{
			open_parens -= 1;
		}
	}

	// Calculate the inverse bind matrix.
	j->_inv_bind = j->_world;
	j->_inv_bind.invert();
	j->_skin = j->_inv_bind * j->_world;
}

void egg_to_animation(const char* filename, ga_animation* animation)
{
	std::ifstream file;
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;
	file.open(fullpath);
	
	char data[128];
	file >> data;
	while (true)
	{
		if (strcmp(data, "<Table>") == 0)
		{
			file >> data;
			char* token = strtok(data, "\"");
			if (strcmp(token, "<skeleton>") == 0)
			{
				break;
			}
		}
		file >> data;
	}

	file >> data;
	int open_parens = 1;

	while (open_parens > 0)
	{
		file >> data;
		if (strcmp(data, "<Table>") == 0)
		{
			parse_joint_anim_data(file, animation);
		}
		else if (strcmp(data, "{") == 0) open_parens += 1;
		else if (strcmp(data, "}") == 0) open_parens -= 1;
	}
}

void parse_joint_anim_data(std::ifstream& file, ga_animation* animation, uint32_t depth)
{
	char data[128];
	int open_parens = 0;

	// Read joint name.
	file >> data;

	file >> data; open_parens += 1;

	while (open_parens > 0)
	{
		file >> data;
		if (strcmp(data, "<Xfm$Anim_S$>") == 0)
		{
			// xform scope.
			file >> data;

			int anim_parens = 0;
			file >> data; anim_parens += 1;

			// We'll store the data in temp vectors and convert them into
			// transform matrices at the end.
			std::vector<float> scale_x;
			std::vector<float> scale_y;
			std::vector<float> scale_z;
			std::vector<float> rotate_r;
			std::vector<float> rotate_p;
			std::vector<float> rotate_h;
			std::vector<float> translate_x;
			std::vector<float> translate_y;
			std::vector<float> translate_z;

			// Lastly, we'll need the order to apply transformations to our
			// final transform matrix.
			char order[10];
			memset(order, 0, sizeof(char) * 10);

			while (anim_parens > 0)
			{
				file >> data;
				if (strcmp(data, "<Scalar>") == 0)
				{
					file >> data; 
					if (strcmp(data, "fps") == 0)
					{
						file >> data; anim_parens += 1;
						file >> data;
						animation->_rate = atoi(data);
						file >> data; anim_parens -= 1;

						if (animation->_poses.size() == 0)
						{
							for (uint32_t i = 0; i < animation->_rate; ++i)
							{
								ga_skeleton_pose empty_pose;
								animation->_poses.push_back(empty_pose);
							}
						}
					}
				}
				else if (strcmp(data, "<Char*>") == 0)
				{
					file >> data;
					if (strcmp(data, "order") == 0)
					{
						file >> data; anim_parens += 1;
						file >> data;
						strcpy(order, data);
						file >> data; anim_parens -= 1;
					}
				}
				else if (strcmp(data, "<S$Anim>") == 0)
				{
					file >> data;
					if (strcmp(data, "i") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							scale_x.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
					else if (strcmp(data, "j") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							scale_y.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
					else if (strcmp(data, "k") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							scale_z.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
					else if (strcmp(data, "r") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							rotate_r.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
					else if (strcmp(data, "p") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							rotate_p.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
					else if (strcmp(data, "h") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							rotate_h.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
					else if (strcmp(data, "x") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							translate_x.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
					else if (strcmp(data, "y") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							translate_y.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
					else if (strcmp(data, "z") == 0)
					{
						file >> data; file >> data; file >> data; file >> data;
						while (strcmp(data, "}") != 0)
						{
							translate_z.push_back((float)atof(data));
							file >> data;
						}
						file >> data;
					}
				}
				else if (strcmp(data, "{") == 0)
				{
					anim_parens += 1;
				}
				else if (strcmp(data, "}") == 0)
				{
					anim_parens -= 1;
				}
			}

			// Now, take all the data for each frame and create transform matrices.
			for (uint32_t frame = 0; frame < animation->_rate; ++frame)
			{
				ga_mat4f pose;
				pose.make_identity();

				for (int i = 0; i < strlen(order); ++i)
				{
					if (order[i] == 's' && scale_x.size() > 0)
					{
						float value = scale_x[frame % scale_x.size()];
						pose.scale(value);
					}
					else if (order[i] == 'r' && rotate_r.size() > 0)
					{
						float value = rotate_r[frame % rotate_r.size()];
						ga_quatf rotation;
						rotation.make_axis_angle(ga_vec3f::z_vector(), ga_degrees_to_radians(value));
						pose.rotate(rotation);
					}
					else if (order[i] == 'p' && rotate_p.size() > 0)
					{
						float value = rotate_p[frame % rotate_p.size()];
						ga_quatf rotation;
						rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(value));
						pose.rotate(rotation);
					}
					else if (order[i] == 'h' && rotate_h.size() > 0)
					{
						float value = rotate_h[frame % rotate_h.size()];
						ga_quatf rotation;
						rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(value));
						pose.rotate(rotation);
					}
					else if (order[i] == 't')
					{
						float x_value = translate_x.size() > 0 ? translate_x[frame % translate_x.size()] : 0.0f;
						float y_value = translate_y.size() > 0 ? translate_y[frame % translate_y.size()] : 0.0f;
						float z_value = translate_z.size() > 0 ? translate_z[frame % translate_z.size()] : 0.0f;
						pose.translate({ x_value, y_value, z_value });
					}
				}

				animation->_poses[frame]._transforms.push_back(pose);
			}
		}
		else if (strcmp(data, "<Table>") == 0)
		{
			parse_joint_anim_data(file, animation, depth + 1);
		}
		else if (strcmp(data, "{") == 0)
		{
			open_parens += 1;
		}
		else if (strcmp(data, "}") == 0)
		{
			open_parens -= 1;
		}
	}
};
