#version 430

layout(location = 0) in vec3 in_vertex;
layout (location=1) in vec2 texCoord;

uniform mat4 u_mvp;

out vec2 tc;
layout (binding=0) uniform sampler2D samp;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;
	tc = texCoord;
}