//[VERTEX SHADER]
#version 330
 
layout( location = 0 ) in vec3 in_vertex;
layout( location = 1 ) in vec3 in_normal;

out vec3 OutNormal;
out vec4 ShadowCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal_matrix;
uniform mat4 depth_bias_MVP;
 
void main()
{
	gl_Position = projection * view * model * vec4( in_vertex, 1.0 );
        ShadowCoord = depth_bias_MVP * vec4( in_vertex, 1.0 );
	vec4 vertex_normal = normal_matrix * vec4( in_normal, 0.0 );
	OutNormal = vertex_normal.xyz;
}