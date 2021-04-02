#version 430

in vec2 tc[];
out vec2 tcs_out[];

uniform mat4 u_mvp;
layout (binding=0) uniform sampler2D u_texture;
layout (vertices = 16) out;

uniform int TL;  // tessellation levels

void main(void)
{	
	if (gl_InvocationID ==0)
	{	gl_TessLevelOuter[0] = TL;
		gl_TessLevelOuter[2] = TL;
		gl_TessLevelOuter[1] = TL;
		gl_TessLevelOuter[3] = TL;
		gl_TessLevelInner[0] = TL;
		gl_TessLevelInner[1] = TL;
	}
	tcs_out[gl_InvocationID] = tc[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}