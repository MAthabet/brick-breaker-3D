#version 330

float ka = 0.5;
float kd = 0.8;
float ks = 5;
float sh = 30;

vec4 object_color = vec4(1,0.3,0.5,1);
vec4 light_color = vec4(0.5,0.8,0.1,1);

in vec3 v;
in vec3 n;
in vec3 l;
in vec3 h;

void main()
{
	vec4 lightIntencity = ka*light_color+
	kd*max(dot(l,n),0)*light_color+
	ks*max(pow(dot(h,n),sh),0)*vec4(1,1,1,1);

	gl_FragColor = object_color * lightIntencity;
}