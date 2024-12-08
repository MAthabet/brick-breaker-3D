#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_normal;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

float ka = 0.5;
float kd = 0.8;
float ks = 5;
float sh = 30;

vec3 light_direction = vec3(1,1,1);
vec4 object_color = vec4(1,1,1,1);
vec4 light_color = vec4(0.6,0.1,1,1);

vec3 v;
vec3 n;
vec3 l;
vec3 h;

vec3 camera_position = vec3(0,0,3);

out vec4 frag_color;
void main()
{
	gl_Position = projMat * viewMat * modelMat * vertex_position;

	l = normalize(light_direction);
	n = normalize(normalize(modelMat*vertex_normal).xyz);
	v = normalize(camera_position-(modelMat*vertex_position).xyz);
	h = normalize(v+l);

	//float I = ka + kd*max(dot(l,n),0);
	vec4 lightIntencity = ka*light_color+
	kd*max(dot(l,n),0)*light_color+
	ks*max(pow(dot(h,n),sh),0)*vec4(1,0,0,1);

	frag_color = object_color * lightIntencity;

}