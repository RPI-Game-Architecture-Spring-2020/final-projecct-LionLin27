#version 400

layout(location = 0) in vec3 in_vertex;

uniform mat4 u_mvp;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;
}