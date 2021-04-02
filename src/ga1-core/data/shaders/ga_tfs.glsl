#version 430

in vec2 tes_out;
out vec4 color;
uniform mat4 u_mvp;

layout (binding=0) uniform sampler2D u_texture;

void main(void)
{
	color = texture(u_texture, tes_out);
}