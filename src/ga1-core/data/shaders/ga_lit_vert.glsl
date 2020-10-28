#version 400

layout(location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcood0;
out vec3 o_normal;
out vec3 o_lightDir;
out vec3 o_vertPos;
out vec2 texcoord0;

uniform mat4 u_mvp;
uniform mat4 u_pMat;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;

	mat4 normMat = transpose(inverse(u_mvp));
	o_normal = normMat * in_normal;
	texcoord0 = in_texcood0;

}