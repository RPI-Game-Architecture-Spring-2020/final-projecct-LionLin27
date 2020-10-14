#version 430

in vec2 tc;
layout (binding=0) uniform sampler2D samp;

void main(void)
{
	gl_FragColor = texture(samp, tc);
}
