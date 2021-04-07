#version 400

in vec3 o_normal;
in vec3 o_color;

struct BaseLight{
	vec3 color;
	float intensity;
};

struct DirectionalLight{
	BaseLight base;
	vec3 direction;
};

uniform vec3 u_baseColor;
uniform vec3 u_ambientLight;
uniform DirectionalLight u_directionalLight;

vec4 calcLight(BaseLight base, vec3 direction, vec3 normal){
	float diffuseFactor = dot(-direction, normal);

	vec4 diffuseColor = vec4(0,0,0,0);
	if(diffuseFactor > 0){
		diffuseColor = vec4(base.color, 1.0) * base.intensity * diffuseFactor;
	}

	return diffuseColor;
}

vec4 calcDirectionalLight(DirectionalLight directionalLight, vec3 normal){
	return calcLight(directionalLight.base, directionalLight.direction, normal);
}

void main(void)
{
	vec4 totalLight = vec4(u_ambientLight, 1);
	vec4 color = vec4(u_baseColor, 1);

	totalLight += calcDirectionalLight(u_directionalLight, o_normal);

	gl_FragColor = color * totalLight;
}
