#version 430

#define POSITIONAL_LIGHTS_MAX 10
#define CUBE_MAP_LODS 7.0

const float PI = 3.14159265359;

uniform sampler2D u_texture;
uniform sampler2D u_normMap;
layout (binding = 5) uniform samplerCube u_envMap;
uniform vec3 u_baseColor;
uniform mat4 u_mvMat;
uniform mat4 u_vMat;

uniform bool b_useNormalMap;
uniform bool b_useTexture;

//uniform bool b_useEnvMap;
uniform float f_roughness;
uniform float f_metalness;
uniform float f_normalStr;

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

float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

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

	// normal strength
	newNormal = normalize(newNormal * f_normalStr + normal * (1.0 - f_normalStr));

	return newNormal;
}

void calcDirectionalLight(DirectionalLight dirLight, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, inout vec3 Lo){

	vec3 L = normalize(dirLight.direction);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);
    vec3 H = normalize(V + L);

    vec3 radiance = dirLight.base.color * dirLight.base.intensity;

    // brdf
    float D = DistributionGGX(N, H, f_roughness);
    float G = GeometrySmith(N, V, L, f_roughness);
    vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - f_metalness;

    vec3 numerator = D*G*F;
    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);  

    float NdotL = max(dot(N,L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
}

void calcPositionalLight(PositionalLight posLight, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, inout vec3 Lo){
	vec3 L = normalize(posLight.position-vertPos);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);
    vec3 H = normalize(V + L);

    float distance = length(posLight.position-vertPos);
    float attenuation = 1.0 / (distance*distance);
    vec3 radiance = posLight.base.color * posLight.base.intensity * attenuation;

    // brdf
    float D = DistributionGGX(N, H, f_roughness);
    float G = GeometrySmith(N, V, L, f_roughness);
    vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - f_metalness;

    vec3 numerator = D*G*F;
    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);  

    float NdotL = max(dot(N,L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
}

void calcEnvReflection(vec4 reflectColor, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, inout vec3 Lo){

}

float lookup(float x, float y)
{  	float t = textureProj(shadowTex, shadow_coord + vec4(x * 0.001 * shadow_coord.w,
                                                         y * 0.001 * shadow_coord.w,
                                                         -0.01, 0.0));
	return t;
}

vec4 blurReflection(vec3 o_vertPos, vec3 o_normal) {
	
	vec3 r = -reflect(normalize(-o_vertPos), normalize(o_normal));
	vec4 r2 = normalize(vec4(r,0) * inverse(u_vMat));

	return textureLod(u_envMap, r2.xyz, f_roughness * CUBE_MAP_LODS);
}

void main(void)
{

	vec3 normal = o_normal;
	
	if(b_useNormalMap){
		normal = calcNewNormal();
	}
    
	// texture
	vec3 baseColor = vec3(1,1,1);
	vec4 textureColor = texture(u_texture, texcoord0);
	vec3 color = baseColor;

	if(textureColor != vec4(0,0,0,0)){
		color *= textureColor.xyz;
	}

    // reflectance eq
    vec3 Lo = vec3(0.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, color, f_metalness);

	calcDirectionalLight(u_directionalLight,  o_vertPos, normal, F0, color, Lo);

    for(int i = 0; i < u_posLightCount; i ++){
        if(i >= POSITIONAL_LIGHTS_MAX){
			break;
		}

        calcPositionalLight(u_positionalLights[i], o_vertPos, normal, F0, color, Lo);
    }


    vec3 ambient = u_ambientLight * color;
    color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    //gl_FragColor = vec4(color, 1.0);

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

/*
	vec3 totalLight = u_ambientLight;
	totalLight += (dirLightDiffuse + posLightDiffuseSum)*inShadow;

	vec4 totalLightv4 = vec4(totalLight, 1.0);
	vec4 totalSpec = vec4(posLightSpecSum+dirLightSpec, 1.0);

*/
	vec4 litColor = vec4(color, 1.0);

	vec4 reflectionColor = blurReflection(o_vertPos, normal);

	vec4 mixedColor = f_metalness * reflectionColor + (1.0 - f_metalness) * litColor;

	gl_FragColor = mix(fogColor, mixedColor, fogFactor);
}