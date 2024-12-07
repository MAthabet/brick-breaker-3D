#include<gl\glew\glew.h>
#include<iostream>
#include<fstream>
using namespace std;

#pragma region SHADER_FUNCTIONS

static char* readFile(const char* fileName)
{
	ifstream shaderfile(fileName, ios::ate);
	if (!shaderfile.is_open())
	{
		cerr << "OpenFileError File:" << fileName;
		return NULL;
	}
	streampos size = shaderfile.tellg();
	char* filedata = new char[size];
	for (int i = 0; i < size; i++)
		filedata[i] = 0;
	shaderfile.seekg(ios::beg);
	shaderfile.read(filedata, size);
	shaderfile.close();
	return filedata;
}

bool compiledStatus(GLint shaderID) {
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled)
		return true;
	else
	{
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf("%s\n", msgBuffer);
		delete(msgBuffer);
		return false;
	}
}

GLuint makeShader(const char* ShaderSourceCode, GLenum SHADER_TYPE)
{
	GLuint vertShaderID = glCreateShader(SHADER_TYPE);
	glShaderSource(vertShaderID, 1, (const GLchar**)&ShaderSourceCode, NULL);
	glCompileShader(vertShaderID);
	bool compiledCorrectly = compiledStatus(vertShaderID);
	if (compiledCorrectly)
		return vertShaderID;
	else
		return -1;
}

GLuint makeShaderProgram(GLuint vertShaderID, GLuint fragShaderID)
{
	GLuint programid = glCreateProgram();
	glAttachShader(programid, vertShaderID);
	glAttachShader(programid, fragShaderID);
	glLinkProgram(programid);
	return programid;
}

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name)
{
	char* vertexShaderSourceCode = readFile(vertex_shader_file_name);
	char* fragmentShaderSourceCode = readFile(fragment_shader_file_name);
	GLuint vertexShaderID = makeShader(vertexShaderSourceCode, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = makeShader(fragmentShaderSourceCode, GL_FRAGMENT_SHADER);
	GLuint shaderProgramID = makeShaderProgram(vertexShaderID, fragmentShaderID);
	printf("Vertex Shader ID:%d", vertexShaderID);
	printf("\nFragment Shader ID:%d", fragmentShaderID);
	printf("\nshaderProgramID:%d", shaderProgramID);
	return shaderProgramID;
}

#pragma endregion SHADER_FUNCTIONS