#version 330

layout (location = 0) in vec4 vertex_position;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;
uniform int c;

out vec4 frag_color;

void main()
{
	gl_Position = projMat * viewMat * modelMat * vertex_position;
	switch (c)
	{
		case 1:
		frag_color = vec4(255.0f/255.0f,208.0f/255.0f,177.0f/255.0f,1);
		break;
		case 2:
		frag_color = vec4(176.0f,120.0f,89.0f,255.0f)/255.0f;
		break;
		case 3:
		frag_color = vec4(105.0f,55.0f,38.0f,255.0f)/255.0f;
		break;
		case 4:
		frag_color = vec4(96.0f,2.0f,21.0f,255.0f)/255.0f;
		break;
		case 5:
		frag_color = vec4(64.0f,1.0f,17.0f,255.0f)/255.0f;
		break;
		default:
		frag_color = vec4(0.15,0.15,0.15,1);
		break;
	}
	if (c == 1)
	frag_color = vec4(255.0f/255.0f,208.0f/255.0f,177.0f/255.0f,1);
}