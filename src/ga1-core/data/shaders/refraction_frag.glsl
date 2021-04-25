#version 430

#define POSITIONAL_LIGHTS_MAX 10
#define CUBE_MAP_LODS 7.0

uniform sampler2D u_texture;
uniform sampler2D u_normMap;
uniform sampler2D u_roughnessMap;
uniform sampler2D u_metallicMap;

layout (binding=11) uniform sampler2D u_backNormals;
layout (binding=10) uniform sampler2D u_backProps;

layout (binding = 5) uniform samplerCube u_envMap;
uniform vec3 u_baseColor;
uniform mat4 u_mvMat;
uniform mat4 u_vMat;
uniform mat4 u_vpMat_proj;

uniform vec3 v_eyePos;

uniform bool b_useTexture;
uniform bool b_useNormalMap;
uniform bool b_useRoughMap;
uniform bool b_useMetalMap;

uniform float f_roughness;
uniform float f_metalness;
uniform float f_normalStr;
uniform float f_ior;

in vec3 o_normal;
in vec3 o_vertPos;
in vec3 o_tangent;
in vec2 texcoord0;
in vec4 shadow_coord;
in vec4 screen_coord;
in vec3 o_worldPos;
in vec3 o_worldNormal;
in vec4 o_internal_dist;

const float PI = 3.14159265359;


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

// REFRACTIOM FUNCTION
vec4 blurRefraction(vec3 vertPos, vec3 normal) {
	vec3 I = normalize(o_worldPos - v_eyePos);
    vec3 T_1 = refract(I, normalize(normal), 1.0 / f_ior);
    // get the depth
    vec4 backNormals = textureProj(u_backNormals, screen_coord);
    // aproximate the piercing internal distance and by-normal internal distance
    float dN = (backNormals.w) - (distance(o_worldPos, v_eyePos));

    mat4 mMat = u_mvMat * inverse(u_vMat);
    float dV = distance((vec4(o_internal_dist.xyz, 1) * mMat).xyz, (vec4(0,0,0, 1) * mMat).xyz);
    dV = dV + 0.01;
    // dV = o_internal_dist.w;
    // float dV = o_internal_dist.w;
    // use angles between incoming and normal and refract and normal
    float theta_i = acos(dot(-I, normalize(normal)));
    float theta_t = acos(dot(T_1, -normalize(normal)));
    float d = ((theta_t / theta_i) * dV) + ((1 - (theta_t / theta_i)) * dN);
    vec3 P_2 = o_worldPos + (T_1 * d);
    // with the estimated position of where T_1 will strike, get second normal
    vec3 N_2 = textureProj(u_backNormals, vec4(P_2, 1) * u_vpMat_proj).xyz;
    int n_good = int(step(0.5, length(N_2)));
    if (n_good < 1) { // poorly correct for out of bounds
        N_2 = textureProj(u_backNormals, screen_coord).xyz;
    }
    // refract by second normal
    vec3 T_2 = refract(T_1, -normalize(N_2), f_ior);
    // If N_2 isn't picked up or T_2 isn't a good refraction, revert to T_1
    float t_good = step(0.01, length(T_2));

    // LOD based on roughness
    float lod = f_roughness;
    if (b_useRoughMap) {
        float back_rough = textureProj(u_backProps, vec4(P_2, 1) * u_vpMat_proj).x;
        float front_rough = texture(u_roughnessMap, texcoord0).x;
        lod = lod * (1 - ((1 - back_rough) * (1 - front_rough)));
    }

	return textureLod(u_envMap, (t_good * T_2) + ((1.0 - t_good) * T_1), CUBE_MAP_LODS * lod);
}

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

vec3 calcNewNormal(vec3 i_normal){
	vec3 normal = normalize(i_normal);
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

void calcDirectionalLight(DirectionalLight dirLight, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metallic, inout vec3 Lo){

	vec3 L = normalize(dirLight.direction);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);
    vec3 H = normalize(V + L);

    vec3 radiance = dirLight.base.color * dirLight.base.intensity;

    // brdf
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = D*G*F;
    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);  

    float NdotL = max(dot(N,L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
}

void calcPositionalLight(PositionalLight posLight, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metallic, inout vec3 Lo){
	vec3 L = normalize(posLight.position-vertPos);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);
    vec3 H = normalize(V + L);

    float distance = length(posLight.position-vertPos);
    float attenuation = 1.0 / (distance*distance);
    vec3 radiance = posLight.base.color * posLight.base.intensity * attenuation;

    // brdf
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = D*G*F;
    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);  

    float NdotL = max(dot(N,L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
}

void calcEnvReflection(vec4 reflectColor, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metallic, inout vec3 Lo){

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
	vec3 worldNormal = o_worldNormal;

	if(b_useNormalMap){
		normal = calcNewNormal(o_normal);
		worldNormal = calcNewNormal(o_worldNormal);
	}

    float roughness = f_roughness;
    if(b_useRoughMap){
        roughness = f_roughness * texture(u_roughnessMap, texcoord0).x;
    }

    float metallic = f_metalness;
    if(b_useMetalMap){
        metallic = f_metalness * texture(u_metallicMap, texcoord0).x;
    }
    
	// texture
	vec3 color = u_baseColor;
    if(b_useTexture){
	    vec4 textureColor = texture(u_texture, texcoord0);
        color *= textureColor.xyz;
    }


    // reflectance eq
    vec3 Lo = vec3(0.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, color, f_metalness);

	calcDirectionalLight(u_directionalLight,  o_vertPos, normal, F0, color, roughness, metallic, Lo);

    for(int i = 0; i < u_posLightCount; i ++){
        if(i >= POSITIONAL_LIGHTS_MAX){
			break;
		}

        calcPositionalLight(u_positionalLights[i], o_vertPos, normal, F0, color, roughness, metallic, Lo);
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

	vec4 litColor = vec4(color, 1.0);

	vec4 refractionColor = blurRefraction(o_vertPos, worldNormal);

	vec4 mixedColor = metallic * refractionColor + (1.0 - metallic) * litColor;

	gl_FragColor = mix(fogColor, mixedColor, fogFactor);
}
