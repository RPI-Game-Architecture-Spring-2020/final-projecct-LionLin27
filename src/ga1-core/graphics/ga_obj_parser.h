#pragma once
#include <vector>

/*
** Read an OBJ file, get the animation data.
void obj_to_animation(const char* filename, struct ga_animation* animation);
*/
/**
* load obj with no animation data
*/
float getDistanceToTri(const ga_vec3f& a, const ga_vec3f& b, const ga_vec3f& c, const ga_vec3f& p, const ga_vec3f& dir);
void obj_to_model(const char* filename, struct ga_model* model);