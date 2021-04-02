#version 430

layout (vertices = 4) out;

uniform mat4 u_mvp;
uniform float u_subdivisions;

in vec2 tc[];
out vec2 tcs_out[];

void main(void)
{	
	//float subdivisions = 16.0;
	int TL = 32;
	if (gl_InvocationID ==0)
	{	
		vec4 p0 = gl_in[0].gl_Position * u_mvp;
		vec4 p1 = gl_in[1].gl_Position * u_mvp;
		vec4 p2 = gl_in[2].gl_Position * u_mvp;
		p0 = p0/p0.w;
		p1 = p1/p1.w;
		p2 = p2/p2.w;

		float width = length(p2.xy - p0.xy) * u_subdivisions + 1.0;
		float height = length(p1.xy - p0.xy) * u_subdivisions + 1.0;
		float avg = (width + height)/2.0;
		gl_TessLevelOuter[0] = avg;
		gl_TessLevelOuter[1] = avg;
		gl_TessLevelOuter[2] = avg;
		gl_TessLevelOuter[3] = avg;
		gl_TessLevelInner[0] = avg;
		gl_TessLevelInner[1] = avg;
	}
	tcs_out[gl_InvocationID] = tc[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}