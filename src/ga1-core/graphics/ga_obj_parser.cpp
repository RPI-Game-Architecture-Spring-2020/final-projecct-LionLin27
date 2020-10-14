/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_obj_parser.h"

//#include "ga_animation.h"
#include "ga_geometry.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>

using namespace std;

std::vector<float> vertVals;
std::vector<float> stVals;
std::vector<float> normVals;

void obj_to_model(const char* filename, ga_model* model)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	float x, y, z;
	string content;
	ifstream fileStream(fullpath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0) {
			stringstream ss(line.erase(0, 1));
			ss >> x; ss >> y; ss >> z;
			vertVals.push_back(x);
			vertVals.push_back(y);
			vertVals.push_back(z);
		}
		if (line.compare(0, 2, "vt") == 0) {
			model->_vertex_format |= k_vertex_attribute_uv;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			stVals.push_back(x);
			stVals.push_back(y);
		}
		if (line.compare(0, 2, "vn") == 0) {
			model->_vertex_format |= k_vertex_attribute_normal;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			normVals.push_back(x);
			normVals.push_back(y);
			normVals.push_back(z);
		}
		if (line.compare(0, 2, "f ") == 0) {
			string oneCorner, v, t, n;
			stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++) {
				getline(ss, oneCorner, ' ');
				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');

				int vertRef = (stoi(v) - 1) * 3;
				int tcRef = (stoi(t) - 1) * 2;
				int normRef = (stoi(n) - 1) * 3;

				ga_vertex v;

				v._position.x = vertVals[vertRef];
				v._position.y = vertVals[vertRef + 1];
				v._position.z = vertVals[vertRef + 2];

				v._uv.x = stVals[tcRef];
				v._uv.y = stVals[tcRef + 1];

				v._normal.x = normVals[normRef];
				v._normal.y = normVals[normRef + 1];
				v._normal.z = normVals[normRef + 2];

				model->_vertices.push_back(v);
			}
		}
	}
}
