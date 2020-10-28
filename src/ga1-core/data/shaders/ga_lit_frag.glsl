#version 400

uniform sampler2D u_texture;
uniform vec3 u_baseColor;

in vec2 texcoord0;

struct BaseLight{
	vec3 color;
	float intensity;
};

struct DirectionalLight{
	BaseLight base;
	vec3 direction;
};

struct PositionalLight{
	BaseLight base;
	vec3 position;
}

uniform vec3 u_ambientLight;
uniform DirectionalLight u_directionalLight;

vec4 calcLight(BaseLight base, vec3 direction, vec3 normal){
	float diffuseFactor = dot(-direction, normal);

	vec4 diffuseColor = vec4(0,0,0,0);
	if(diffuseFactor < 0){
		diffuseFactor = 0;
	}
	diffuseColor = vec4(base.color, 1.0) * base.intensity * diffuseFactor;

	return diffuseColor;
}

vec4 calcDirectionalLight(DirectionalLight directionalLight, vec3 normal){
	return calcLight(directionalLight.base, directionalLight.direction, normal);
}

void main(void)
{
	vec4 totalLight = vec4(u_ambientLight, 1);
	vec4 textureColor = texture(u_texture, texcoord0);
	vec4 color = vec4(u_baseColor, 1);

	if(textureColor != vec4(0,0,0,0)){
		color *= textureColor;
	}
	gl_FragColor = color * totalLight;
}