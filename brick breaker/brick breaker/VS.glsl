#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_color;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec4 frag_color;
void main()
{
	gl_Position = projMat * viewMat * modelMat * vertex_position;
	frag_color = vertex_color;
}