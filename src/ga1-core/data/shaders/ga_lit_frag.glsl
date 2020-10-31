#version 400

#define POSITIONAL_LIGHTS_MAX 10

uniform sampler2D u_texture;
uniform vec3 u_baseColor;

in vec3 o_normal;
in vec3 o_vertPos;
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
};

uniform vec3 u_ambientLight;
uniform DirectionalLight u_directionalLight;
uniform float u_posLightCount;
uniform PositionalLight u_positionalLights[POSITIONAL_LIGHTS_MAX];

vec3 calcDirectionalLight(DirectionalLight dirLight, vec3 normal, vec3 vertPos){
	vec3 L = normalize(dirLight.direction);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);

	// compute light reflection vector
	vec3 R = normalize(reflect(-L, N));

	// angle between light and normal
	float cosTheta = dot(L,N);

	// angle between view vector and reflected light
	float cosPhi = dot(V,R);

	// compute ADS
	vec3 diffuse = dirLight.base.color.xyz * max(cosTheta, 0.0);
	vec3 specular = dirLight.base.color.xyz * pow(max(cosPhi,0.0), 51);//material.shininess);

	return (diffuse + specular) * dirLight.base.intensity;
}

vec3 calcPositionalLight(PositionalLight posLight, vec3 normal, vec3 vertPos){
	vec3 L = normalize(posLight.position);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);

	// compute light reflection vector
	vec3 R = normalize(reflect(-L, N));

	// angle between light and normal
	float cosTheta = dot(L,N);

	// angle between view vector and reflected light
	float cosPhi = dot(V,R);

	// compute ADS
	vec3 diffuse = posLight.base.color.xyz * max(cosTheta, 0.0);
	vec3 specular = posLight.base.color.xyz * pow(max(cosPhi,0.0), 51);//material.shininess);

	return (diffuse + specular) * posLight.base.intensity;
}

void main(void)
{
	vec3 dirLight = calcDirectionalLight(u_directionalLight, o_normal, o_vertPos);
	vec3 posLight = vec3(0,0,0);
	for (int i = 0; i < u_posLightCount; i ++){
		if(i >= POSITIONAL_LIGHTS_MAX){
			break;
		}
		posLight += calcPositionalLight(u_positionalLights[i], o_normal, o_vertPos);
	}
	vec4 totalLight = vec4(u_ambientLight + dirLight + posLight, 1);

	vec3 baseColor = vec3(1,1,1);
	vec4 textureColor = texture(u_texture, texcoord0);
	vec4 color = vec4(baseColor, 1);

	if(textureColor != vec4(0,0,0,0)){
		color *= textureColor;
	}
	gl_FragColor = color * totalLight;
}