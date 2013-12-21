//[VERTEX SHADER]
#version 330
 
in vec3 in_vertex;
in vec3 in_normal;

out vec3 OutNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal_matrix;
 
void main()
{
	gl_Position = projection * view * model * vec4( in_vertex, 1.0 );
	vec4 vertex_normal = normal_matrix * vec4( in_normal, 1.0 );
	OutNormal = vertex_normal.xyz;
}