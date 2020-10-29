#version 400

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

/* modify these to support multiple light source
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
*/

void main(void)
{
	// normalize light normal and view
	vec3 L = normalize(u_directionalLight.direction);
	vec3 N = normalize(o_normal);
	vec3 V = normalize(-o_vertPos);

	// compute light reflection vector
	vec3 R = normalize(reflect(-L, N));

	// angle between light and normal
	float cosTheta = dot(L,N);

	// angle between view vector and reflected light
	float cosPhi = dot(V,R);

	// compute ADS
	vec3 diffuse = u_directionalLight.base.color.xyz * max(cosTheta, 0.0);
	vec3 specular = u_directionalLight.base.color.xyz * pow(max(cosPhi,0.0), 51);//material.shininess);

	vec3 baseColor = vec3(1,1,1);
	vec4 totalLight = vec4(u_ambientLight + diffuse + specular, 1);
	vec4 textureColor = texture(u_texture, texcoord0);
	vec4 color = vec4(baseColor, 1);

	if(textureColor != vec4(0,0,0,0)){
		color *= textureColor;
	}
	gl_FragColor = color * totalLight;
}