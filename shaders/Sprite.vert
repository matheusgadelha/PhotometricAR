#version 330
 
in vec2 in_vertex;
in vec2 in_texcoord;

out vec2 out_texcoord;
 
void main()
{
	out_texcoord = in_texcoord;
	gl_Position = vec4( in_vertex, 0.0, 1.0 );
}