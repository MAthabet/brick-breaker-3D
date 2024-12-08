#version 330

layout (location = 0) in vec4 vertex_position;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;
uniform float c;

out vec4 frag_color;

void main()
{
	gl_Position = projMat * viewMat * modelMat * vertex_position;
	if (c==0)
	frag_color = vec4(0,0,0,1);
	else
	frag_color = vec4(1/(c+0.2),1/(c+0.3),1/c,1);
}