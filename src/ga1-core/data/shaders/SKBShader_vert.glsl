#version 430

layout (location = 0) in vec3 position;
out vec3 tc;

uniform mat4 u_vMat;
uniform mat4 u_pMat;
layout (binding = 5) uniform samplerCube samp;

void main(void)
{
	tc = position;
	mat4 v3_matrix = mat4(mat3(u_vMat));
	gl_Position = vec4(position,1.0) * v3_matrix * u_pMat;
}
