#version 430

uniform vec3 u_baseColor;
uniform mat4 u_mvMat;
uniform mat4 u_vMat;

uniform vec3 v_eyePos;

uniform sampler2D u_texture;
uniform sampler2D u_normMap;
uniform sampler2D u_roughnessMap;
uniform sampler2D u_metallicMap;

uniform bool b_useTexture;
uniform bool b_useNormalMap;
uniform bool b_useRoughMap;
uniform bool b_useMetalMap;

//uniform bool b_useEnvMap;
uniform float f_roughness;
uniform float f_metalness;
uniform float f_normalStr;

in vec3 o_normal;
in vec3 o_vertPos;
in vec3 o_tangent;
in vec2 texcoord0;
in vec3 o_worldPos;

layout(location = 0) out vec4 normalTex;
layout(location = 1) out vec2 propTex;

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

	float roughness = f_roughness;
	float metallic = f_metalness;
	if (b_useRoughMap) {
		roughness = roughness * texture(u_roughnessMap, texcoord0).x;
	}
	if (b_useMetalMap) {
		metallic = metallic * texture(u_metallicMap, texcoord0).x;
	}

	normalTex = vec4(normal.xyz, (distance(o_worldPos, v_eyePos)));
	// TODO : after you get everything else working
	propTex = vec2(roughness, metallic);
}