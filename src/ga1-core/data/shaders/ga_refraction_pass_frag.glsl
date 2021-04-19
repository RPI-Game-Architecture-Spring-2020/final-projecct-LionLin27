#version 430

uniform sampler2D u_normMap;
uniform vec3 u_baseColor;
uniform mat4 u_mvMat;
uniform mat4 u_vMat;

uniform vec3 v_eyePos;
uniform bool b_useNormalMap;
uniform float f_normalStr;
uniform float u_roughness;

in vec3 o_normal;
in vec3 o_vertPos;
in vec3 o_tangent;
in vec2 texcoord0;
in vec3 o_worldPos;

layout(location = 0) out vec4 normalTex;
layout(location = 1) out float roughnessTex;

vec3 calcNewNormal() {
	vec3 normal = normalize(o_normal);
	vec3 tangent = normalize(o_tangent);
	tangent = normalize(tangent - dot(tangent, normal)*normal);
	vec3 btangent = cross(tangent, normal);
	mat3 tbn = mat3(tangent, btangent, normal);

	vec3 map_normal = texture(u_normMap, texcoord0).xyz;
	map_normal = map_normal * 2.0 - 1.0;
	vec3 newNormal = tbn * map_normal;
	newNormal = normalize(newNormal);

	// normal strength
	newNormal = normalize(newNormal * f_normalStr + normal * (1.0 - f_normalStr));

	return newNormal;
}

void main(void)
{
	vec3 normal = o_normal;
	
	if (b_useNormalMap) {
		normal = calcNewNormal();
	}

	normalTex = vec4(normal.xyz, (distance(o_worldPos, v_eyePos)));
	roughnessTex = u_roughness;
}