#version 430

#define POSITIONAL_LIGHTS_MAX 10
#define CUBE_MAP_LODS 7.0

const float PI = 3.14159265359;

uniform sampler2D u_texture;
uniform sampler2D u_normMap;
uniform sampler2D u_roughnessMap;
uniform sampler2D u_metallicMap;
uniform sampler2D u_AOMap;
layout (binding = 5) uniform samplerCube u_envMap;
uniform vec3 u_baseColor;
uniform mat4 u_mvMat;
uniform mat4 u_vMat;

uniform bool b_useTexture;
uniform bool b_useNormalMap;
uniform bool b_useEnvMap;
uniform bool b_useRoughMap;
uniform bool b_useMetalMap;
uniform bool b_useAOMap;

uniform float f_roughness;
uniform float f_metalness;
uniform float f_normalStr;

uniform bool b_NDF;
uniform bool b_GEO;
uniform bool b_FNL;
uniform int u_debug;
uniform int u_NDFSelection;
uniform int u_GEOSelection;
uniform int u_FNLSelection;

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

layout (binding=9) uniform sampler2DShadow shadowTex;

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

float DistributionBlinnPhong(vec3 N, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.00001);

    float power = 2.0/a2 -2.0;
    float scaler = 1.0/PI/a2;
    //if(NdotH <= 0) return 0.0001;

    return scaler * pow(NdotH, power);
}

float DistributionBeckmann(vec3 N, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = dot(N, H);//max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float NdotH4 = NdotH2 * NdotH2;

    float base = 1.0/PI/a2/NdotH4;
    float power = (NdotH2-1) / (a2 * NdotH2);

    return pow(base, power); 
}

float GeometryBeckmann(float NdotV, float roughness){
    float a = roughness * roughness;
    float c = NdotV / (a * sqrt(1-NdotV*NdotV));

    if(c < 1.6){
        float c2 = c*c;
        float num = 2.181*c2 + 3.535*c;
        float denum = 2.577*c2 + 2.276*c + 1;

        return num / denum;
    }else{
        return 1.0;
    }
}

float GeometrySchlickBeckmann(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = roughness;
    float k = (r*r) * sqrt(2.0/PI);

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySchlickGGX2(float NdotV, float roughness){
    float r = roughness;
    float k = (r*r) / 2.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    
    if(u_GEOSelection == 2){
        ggx2  = GeometrySchlickGGX2(NdotV, roughness);
        ggx1  = GeometrySchlickGGX2(NdotL, roughness);
    }else if (u_GEOSelection == 3){
        ggx2  = GeometrySchlickBeckmann(NdotV, roughness);
        ggx1  = GeometrySchlickBeckmann(NdotL, roughness);
    }else if (u_GEOSelection == 4){
        ggx2  = GeometryBeckmann(NdotV, roughness);
        ggx1  = GeometryBeckmann(NdotL, roughness);
    }
	
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

void calcDirectionalLight(DirectionalLight dirLight, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metallic, inout vec3 Lo, out vec3 spec_debug){

    vec3 L = normalize(dirLight.direction);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);
    vec3 H = normalize(V + L);

    vec3 radiance = dirLight.base.color * dirLight.base.intensity;

    // brdf
    float D = DistributionGGX(N, H, roughness);
    if(u_NDFSelection == 2){
        D = DistributionBlinnPhong(N,H,roughness);
    }else if (u_NDFSelection == 3){
        D = DistributionBeckmann(N,H,roughness);
    }

    // geometry selection is done in smith
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(N,V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    //vec3 numerator = D*G*F;
    // spit steps for visualization 
    vec3 numerator = vec3(1.0);
    if(b_NDF)
        numerator *= D;
    if(b_GEO)
        numerator *= G;
    if(b_FNL)
        numerator *= F;

    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);  

    float NdotL = max(dot(N,L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    spec_debug += specular * NdotL;
}

void calcPositionalLight(PositionalLight posLight, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metallic, inout vec3 Lo, out vec3 spec_debug){
    
    vec3 L = normalize(posLight.position-vertPos);
	vec3 N = normalize(normal);
	vec3 V = normalize(-vertPos);
    vec3 H = normalize(V + L);

    float distance = length(posLight.position-vertPos);
    float attenuation = 1.0 / (distance*distance);
    vec3 radiance = posLight.base.color * posLight.base.intensity * attenuation;

    // brdf
    float D = DistributionGGX(N, H, roughness);
    if(u_NDFSelection == 2){
        D = DistributionBlinnPhong(N,H,roughness);
    }else if (u_NDFSelection == 3){
        D = DistributionBeckmann(N,H,roughness);
    }
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(N,V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    //vec3 numerator = D*G*F;
    vec3 numerator = vec3(1.0);
    if(b_NDF)
        numerator *= D;
    if(b_GEO)
        numerator *= G;
    if(b_FNL)
        numerator *= F;

    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denom, 0.001);  

    float NdotL = max(dot(N,L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    spec_debug += specular * NdotL;
}

void calcEnvReflection(vec4 reflectColor, vec3 vertPos, vec3 normal, vec3 F0, vec3 albedo, float roughness, float metallic, inout vec3 Lo){

}

float lookup(float x, float y)
{  	float t = textureProj(shadowTex, shadow_coord 
            + vec4(x * (1.0/1280.0) * shadow_coord.w,
                   y * (1.0/720.0) * shadow_coord.w,
                   0.0, 0.0));
	return t;
}

vec4 blurReflection(vec3 o_vertPos, vec3 o_normal, float roughness) {
	
	vec3 r = -reflect(normalize(-o_vertPos), normalize(o_normal));
	vec4 r2 = normalize(vec4(r,0) * inverse(u_vMat));

	return textureLod(u_envMap, r2.xyz, roughness * CUBE_MAP_LODS);
}

void main(void)
{

	vec3 normal = o_normal;
	
	if(b_useNormalMap){
		normal = calcNewNormal();
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
    //F0 = mix(F0, color, f_metalness);
    F0 = mix(F0, vec3(1.0), metallic);

    vec3 spec_debug = vec3(0.0);
    vec3 spec_debug2 = vec3(0.0);
	calcDirectionalLight(u_directionalLight,  o_vertPos, normal, F0, color, roughness, metallic, Lo, spec_debug);

    for(int i = 0; i < u_posLightCount; i ++){
        if(i >= POSITIONAL_LIGHTS_MAX){
			break;
		}

        calcPositionalLight(u_positionalLights[i], o_vertPos, normal, F0, color, roughness, metallic, Lo, spec_debug2);
    }


    vec3 ambient = u_ambientLight * color;
    color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

	//shadow
	float shadowFactor = 0.0;
	float swidth = 2.5;

    vec2 o = mod(floor(gl_FragCoord.xy), 2.0) * swidth;
    shadowFactor += lookup(-1.0*swidth + o.x,  1.0*swidth - o.y);
    shadowFactor += lookup(-1.0*swidth + o.x, -0.3*swidth - o.y);
    shadowFactor += lookup( 0.3*swidth + o.x,  1.0*swidth - o.y);
    shadowFactor += lookup( 0.3*swidth + o.x, -0.3*swidth - o.y);
    shadowFactor = shadowFactor / 4.0;

    color += shadowFactor * vec3(5.0*ambient);

    if(b_useAOMap){
        color *= texture(u_AOMap, texcoord0).x;
    }

	// fog
	vec4 fogColor = vec4(0.7, 0.8, 0.9, 1.0);
	float fogStart = 10;
	float fogEnd = 100;

	vec3 eyePos = o_vertPos;
	float dist = length(eyePos);
	float fogFactor = clamp(((fogEnd - dist) / (fogEnd - fogStart)), 0.0, 1.0);

	vec4 litColor = vec4(color, 1.0);

	vec4 reflectionColor = blurReflection(o_vertPos, normal, roughness);

    vec4 mixedColor = litColor;
    if(b_useEnvMap){
        //vec3 F = fresnelSchlick(max(dot(normalize(normal),normalize(-o_vertPos)), 0.0), F0);
        //vec3 F2 = vec3(1.0,1.0,1.0)-F;
	    //mixedColor = vec4(F,1.0) * reflectionColor + vec4(F2,1.0) * litColor;
        mixedColor = metallic * reflectionColor + (1.0-metallic) * litColor;
    }

    if(u_debug == 0)
	    gl_FragColor = mix(fogColor, mixedColor, fogFactor);
    else if (u_debug == 1)
        gl_FragColor = vec4(spec_debug,1.0);
    else if(u_debug == 2)
        gl_FragColor = vec4(o_normal, 1.0);
    else if(u_debug == 3)
        gl_FragColor = vec4(normal, 1.0);
    else if(u_debug == 4)
        gl_FragColor = texture(u_AOMap, texcoord0);
    else if(u_debug == 6)
        gl_FragColor = vec4(shadowFactor,shadowFactor,shadowFactor, 1.0);
    else if(u_debug == 7){
        shadowFactor = lookup( 0.0,0.0);
        gl_FragColor = vec4(shadowFactor,shadowFactor,shadowFactor, 1.0);
    }
    else
        gl_FragColor = mix(fogColor, mixedColor, fogFactor);
}