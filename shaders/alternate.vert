//[VERTEX SHADER]
#version 330
 
in vec3 in_vertex;
in vec3 in_normal;
 
void main()
{
	gl_Position = vec4( in_vertex, 1.0 );
}