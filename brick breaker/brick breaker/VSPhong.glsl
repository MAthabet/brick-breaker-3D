#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_normal;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;


out vec3 v;
out vec3 n;
out vec3 l;
out vec3 h;

vec3 camera_position = vec3(0,2,5);
vec3 light_direction = vec3(0,0,0);

void main()
{
	gl_Position = projMat * viewMat * modelMat * vertex_position;

	l = normalize(light_direction);
	n = normalize(normalize(modelMat*vec4(vertex_normal)).xyz);
	v = normalize(camera_position-(modelMat*vec4(vertex_position)).xyz);
	h = normalize(v+l);

}