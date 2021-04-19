#version 430

layout(location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normal;
layout(location = 3) in vec2 in_texcood0;
layout(location = 4) in vec3 in_tangent;
out vec3 o_normal;
out vec3 o_vertPos;
out vec3 o_tangent;
out vec3 o_worldPos;
out vec2 texcoord0;

uniform mat4 u_mvp;
uniform mat4 u_mvMat;
uniform mat4 u_vMat;


uniform bool b_useNormalMap;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;

	mat4 normMat = transpose(inverse(u_mvMat * inverse(u_vMat)));
	o_normal = (vec4(in_normal,1.0) * normMat).xyz;
	o_vertPos = (vec4(in_vertex, 1.0) * u_mvMat).xyz;
	vec4 inter = (vec4(in_vertex, 1.0) * u_mvMat);
	o_worldPos = (inter * inverse(u_vMat)).xyz;
	texcoord0 = in_texcood0;

	if(b_useNormalMap){
		o_tangent = in_tangent;
	}else{
		o_tangent = vec3(0,0,0);
	}
}