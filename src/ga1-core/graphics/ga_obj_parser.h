#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/
#include <vector>

/*
** Read an OBJ file, get the animation data.
void obj_to_animation(const char* filename, struct ga_animation* animation);
*/
void obj_to_model(const char* filename, struct ga_model* model);