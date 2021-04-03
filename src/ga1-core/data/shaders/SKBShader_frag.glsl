#version 430

in vec3 tc;
out vec4 fragColor;

layout (binding = 5) uniform samplerCube samp;

void main(void)
{
	fragColor = texture(samp,tc);
}
