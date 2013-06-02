//[VERTEX SHADER]
#version 330
 
in vec3 in_vertex;
in vec3 in_normal;

out vec3 OutNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
 
void main()
{
	gl_Position = projection * view * vec4( in_vertex, 1.0 );
	OutNormal = in_normal;
}