#version 430

#define POSITIONAL_LIGHTS_MAX 10

uniform sampler2D u_texture;
uniform sampler2D u_normMap;
uniform vec3 u_baseColor;
uniform mat4 u_mvMat;

uniform bool b_useNormalMap;
uniform bool b_useTexture;

in vec3 o_normal;
in vec3 o_vertPos;
in vec3 o_tangent;
in vec2 texcoord0;
in vec4 shadow_coord;

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

layout (binding=1) uniform sampler2DShadow shadowTex;

vec3 calcNewNormal(){
	vec3 normal = normalize(o_normal);
	vec3 tangent = normalize(o_tangent);
	tangent = normalize(tangent - dot(tangent, normal)*normal);
	vec3 btangent = cross(tangent, normal);
	mat3 tbn = mat3(tangent, btangent, normal);

	vec3 map_normal = texture(u_normMap, texcoord0).xyz;
	map_normal = map_normal * 2.0 - 1.0;
	vec3 newNormal = tbn * map_normal;
	newNormal = normalize(newNormal);
	return newNormal;
}

void calcDirectionalLight(DirectionalLight dirLight, vec3 normal, vec3 vertPos, 
	out vec3 diffuse, out vec3 specular){
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
	diffuse =  dirLight.base.intensity * dirLight.base.color.xyz * max(cosTheta, 0.0);
	specular = dirLight.base.intensity * dirLight.base.color.xyz * pow(max(cosPhi,0.0), 51);//material.shininess);

	//return (diffuse + specular) * dirLight.base.intensity;
}

void calcPositionalLight(PositionalLight posLight, vec3 normal, vec3 vertPos,
	out vec3 diffuse, out vec3 specular){
	vec3 L = normalize(posLight.position-vertPos);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);

	// compute light reflection vector
	vec3 R = normalize(reflect(-L, N));

	// angle between light and normal
	float cosTheta = dot(L,N);

	// angle between view vector and reflected light
	float cosPhi = dot(V,R);

	// compute ADS
	diffuse = posLight.base.color.xyz * max(cosTheta, 0.0);
	specular = posLight.base.color.xyz * pow(max(cosPhi,0.0), 51);//material.shininess);

	float dist = length(posLight.position-vertPos);
	float intensity = max(posLight.base.intensity - dist*0.1, 0);

	diffuse *= intensity;
	specular *= intensity;
	//return (diffuse + specular) * intensity;
}

float lookup(float x, float y)
{  	float t = textureProj(shadowTex, shadow_coord + vec4(x * 0.001 * shadow_coord.w,
                                                         y * 0.001 * shadow_coord.w,
                                                         -0.01, 0.0));
	return t;
}

void main(void)
{
	vec3 normal = o_normal;
	
	if(b_useNormalMap){
		normal = calcNewNormal();
	}

	vec3 dirLightDiffuse;
	vec3 dirLightSpec;
	calcDirectionalLight(u_directionalLight, normal, o_vertPos, dirLightDiffuse, dirLightSpec);

	vec3 posLightDiffuseSum = vec3(0,0,0);
	vec3 posLightSpecSum = vec3(0,0,0);
	for (int i = 0; i < u_posLightCount; i ++){
		if(i >= POSITIONAL_LIGHTS_MAX){
			break;
		}

		vec3 posLightDiffise;
		vec3 posLightSpec;

		calcPositionalLight(u_positionalLights[i], normal, o_vertPos, posLightDiffise, posLightSpec);

		posLightDiffuseSum += posLightDiffise;
		posLightSpecSum += posLightSpec;
	}

	// texture
	vec3 baseColor = vec3(1,1,1);
	vec4 textureColor = texture(u_texture, texcoord0);
	vec4 color = vec4(baseColor, 1);

	if(textureColor != vec4(0,0,0,0)){
		color *= textureColor;
	}

	//shadow
	float shadowFactor=0.0;
	float swidth = 2.5;
	vec2 o = mod(floor(gl_FragCoord.xy), 2.0) * swidth;
	shadowFactor += lookup(-1.5*swidth + o.x,  1.5*swidth - o.y);
	shadowFactor += lookup(-1.5*swidth + o.x, -0.5*swidth - o.y);
	shadowFactor += lookup( 0.5*swidth + o.x,  1.5*swidth - o.y);
	shadowFactor += lookup( 0.5*swidth + o.x, -0.5*swidth - o.y);
	shadowFactor = shadowFactor / 4.0;

	float inShadow = textureProj(shadowTex, shadow_coord);

	// fog
	vec4 fogColor = vec4(0.7, 0.8, 0.9, 1.0);
	float fogStart = 10;
	float fogEnd = 100;

	vec3 eyePos = o_vertPos;
	float dist = length(eyePos);
	float fogFactor = clamp(((fogEnd - dist) / (fogEnd - fogStart)), 0.0, 1.0);

	vec3 totalLight = u_ambientLight;
	totalLight += (dirLightDiffuse + posLightDiffuseSum)*inShadow;

	vec4 totalLightv4 = vec4(totalLight, 1.0);
	vec4 totalSpec = vec4(posLightSpecSum+dirLightSpec, 1.0);

	gl_FragColor = mix(fogColor, color * totalLightv4 + totalSpec, fogFactor);
}