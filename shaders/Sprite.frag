#version 330

in vec2 out_texcoord;

uniform sampler2D Tex1;

out vec4 FragColor;

void main()
{
	vec4 tex_color = texture( Tex1, out_texcoord );
	FragColor = tex_color;
}