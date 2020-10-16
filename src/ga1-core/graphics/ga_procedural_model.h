#pragma once

#include "math/ga_math.h"

/*
generate a sphere model procedurally
*/
void generate_sphere(int prec, struct ga_model* model);

void generate_torus(float in, float out, int precIn, struct ga_model* model);