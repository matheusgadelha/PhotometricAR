#version 330 core
 
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 in_vertex;
 
// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;
 
void main(){
    gl_Position =  depthMVP * vec4(in_vertex, 1.0f);
}