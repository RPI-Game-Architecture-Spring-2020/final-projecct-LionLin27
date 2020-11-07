#version 430

in vec3 tc;
out vec4 fragColor;

layout (binding = 0) uniform samplerCube samp;

void main(void)
{
	fragColor = texture(samp,tc);
}
