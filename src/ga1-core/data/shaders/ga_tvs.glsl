#version 430

out vec2 tc;

uniform mat4 u_mvp;
uniform mat4 u_controls;
layout (binding = 0) uniform sampler2D u_texture;

void main(void)
{	const vec4 vertices[] =
		vec4[] (vec4(-1.0, u_controls[0][0], -1.0, 1.0),
				vec4(-0.5, u_controls[0][1], -1.0, 1.0),
				vec4( 0.5, u_controls[0][2], -1.0, 1.0),
				vec4( 1.0, u_controls[0][3], -1.0, 1.0),
				
				vec4(-1.0, u_controls[1][0], -0.5, 1.0),
				vec4(-0.5, u_controls[1][1], -0.5, 1.0),
				vec4( 0.5, u_controls[1][2], -0.5, 1.0),
				vec4( 1.0, u_controls[1][3], -0.5, 1.0),
				
				vec4(-1.0, u_controls[2][0],  0.5, 1.0),
				vec4(-0.5, u_controls[2][1],  0.5, 1.0),
				vec4( 0.5, u_controls[2][2],  0.5, 1.0),
				vec4( 1.0, u_controls[2][3],  0.5, 1.0),
				
				vec4(-1.0, u_controls[3][0],  1.0, 1.0),
				vec4(-0.5, u_controls[3][1],  1.0, 1.0),
				vec4( 0.5, u_controls[3][2],  1.0, 1.0),
				vec4( 1.0, u_controls[3][3],  1.0, 1.0));
				
	tc = vec2((vertices[gl_VertexID].x + 1.0)/2.0, (vertices[gl_VertexID].z + 1.0)/2.0);

	gl_Position = vertices[gl_VertexID];
}