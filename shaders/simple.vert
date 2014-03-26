//[VERTEX SHADER]
#version 330
 
layout( location = 0 ) in vec3 in_vertex;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in float in_visibility;

out vec3 OutNormal;
out float OutVisibility;
out vec4 ShadowCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal_matrix;
uniform mat4 depth_bias_MVP;
 
void main()
{
        OutVisibility = in_visibility;
	gl_Position = projection * view * model * vec4( in_vertex, 1.0 );
        ShadowCoord = depth_bias_MVP * vec4( in_vertex, 1.0 );
	vec4 vertex_normal = normal_matrix * vec4( in_normal, 0.0 );
	OutNormal = normalize(vertex_normal.xyz);
}
