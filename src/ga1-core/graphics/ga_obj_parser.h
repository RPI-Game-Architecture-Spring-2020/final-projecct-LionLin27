#pragma once
#include <vector>

/*
** Read an OBJ file, get the animation data.
void obj_to_animation(const char* filename, struct ga_animation* animation);
*/
/**
* load obj with no animation data
*/
void obj_to_model(const char* filename, struct ga_model* model);