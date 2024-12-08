#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>

using namespace std;
using namespace glm;

struct vertex
{
	vec3 position;
	vec3 color;
	vertex() {}
	vertex(vec3 _position, vec3 _color) :position{ _position }, color{ _color }
	{
	}
	vertex(vec3 _position) :position{ _position }, color{ _position }
	{
	}
};

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VBO_Ball, VBO_Paddle, VBO_Wall, VBO_Box, IBO;
GLuint BasiceprogramId, phongProgramId, smoothProgramId, boxProgramId;
float mousposX;
// transformation
GLuint modelMatLoc, viewMatLoc, projMatLoc;
float c[6][6] = {
	{1,1,1,1,1,1},
	{2,2,2,2,2,2},
	{0,3,3,3,3,0},
	{0,4,4,4,4,0},
	{0,5,1,5,5,0},
	{0,6,2,6,3,0}
};
bool firstStart = true;

int vertices_Indeces[] = {
	//front
	0,
	1,
	2,

	0,
	2,
	3,
	//Right
	3,
	2,
	5,

	3,
	5,
	4,
	//Back
	4,
	5,
	6,

	4,
	6,
	7,
	//Left
	7,
	6,
	1,

	7,
	1,
	0,
	//Top
	7,
	0,
	3,

	7,
	3,
	4,
	//Bottom
	2,
	1,
	6,

	2,
	6,
	5
};

#pragma region Paddle


vertex paddleVertices[];

void CreatePaddle(float posX)
{
	const float paddleW = 1, paddleH = 0.2, paddleD = 0.2;

	vec3 center(posX,0,0);
	vertex paddleVertices[] = 
	{
		{vec3(center.x - paddleW / 2, center.y + paddleH / 2, center.z + paddleD / 2),vec3(1,0,0)},
		{vec3(center.x - paddleW / 2, center.y - paddleH / 2, center.z + paddleD / 2),vec3(0,1,0)},
		{vec3(center.x + paddleW / 2, center.y - paddleH / 2, center.z + paddleD / 2),vec3(1,0,1)},
		{vec3(center.x + paddleW / 2, center.y + paddleH / 2, center.z + paddleD / 2),vec3(0,0,1)},
		{vec3(center.x + paddleW / 2, center.y + paddleH / 2, center.z - paddleD / 2),vec3(1,1,0)},
		{vec3(center.x + paddleW / 2, center.y - paddleH / 2, center.z - paddleD / 2),vec3(0,1,1)},
		{vec3(center.x - paddleW / 2, center.y - paddleH / 2, center.z - paddleD / 2),vec3(1,1,1)},
		{vec3(center.x - paddleW / 2, center.y + paddleH / 2, center.z - paddleD / 2),vec3(0,0,0)}
	};
	// create VBO
	glGenBuffers(1, &VBO_Paddle);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Paddle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(paddleVertices), paddleVertices, GL_DYNAMIC_DRAW);

	// Index Buffer
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_Indeces), vertices_Indeces, GL_DYNAMIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);

}

void BindPaddle()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Paddle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);
}

#pragma endregion 

#pragma region Ball
vector<vertex> sphere_vertices;
void Triangle(vec3 a, vec3 b, vec3 c)
{
	vec3 normal = (a + b + c) / 3.0f;
	sphere_vertices.push_back(vertex(a, a));
	sphere_vertices.push_back(vertex(b, b));
	sphere_vertices.push_back(vertex(c, c));
}
void dividTriangle(vec3 a, vec3 b, vec3 c, int itertions)
{
	if (itertions > 0)
	{
		vec3 v1 = normalize(a + b);
		vec3 v2 = normalize(a + c);
		vec3 v3 = normalize(b + c);

		dividTriangle(a, v1, v2, itertions - 1);
		dividTriangle(v1, b, v3, itertions - 1);
		dividTriangle(v1, v3, v2, itertions - 1);
		dividTriangle(v2, v3, c, itertions - 1);
	}
	else
	{
		Triangle(a, b, c);
	}
}
void CreateSphere(int iterations)
{
	vec3 Sphere_Core_vertices[4] = {
		vec3(0.0, 0.0, 1.0),
		vec3(0.0, 0.942809, -0.333333),
		vec3(-0.816497, -0.471405, -0.333333),
		vec3(0.816497, -0.471405, -0.333333)
	};

	sphere_vertices.clear();
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[1], Sphere_Core_vertices[2], iterations);
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[3], Sphere_Core_vertices[1], iterations);
	dividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[2], Sphere_Core_vertices[3], iterations);
	dividTriangle(Sphere_Core_vertices[3], Sphere_Core_vertices[2], Sphere_Core_vertices[1], iterations);


	glGenBuffers(1, &VBO_Ball);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Ball);
	glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(vertex), sphere_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);
}
void BindSphere()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Ball);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);
}
#pragma endregion 

#pragma region Wall

vertex wallVertices[];

void CreateWall()
{
	const float WallW = 6, WallH = 6.5, WallD = 0.2;
	vec3 center(0, 0, 0);
	vec3 color(0, 0.5, 0.5);
	vertex wallVertices[] =
	{
		{vec3(center.x - WallW / 2, center.y + WallH / 2, center.z + WallD / 2),color},
		{vec3(center.x - WallW / 2, center.y - WallH / 2, center.z + WallD / 2),color},
		{vec3(center.x + WallW / 2, center.y - WallH / 2, center.z + WallD / 2),color},
		{vec3(center.x + WallW / 2, center.y + WallH / 2, center.z + WallD / 2),color},
		{vec3(center.x + WallW / 2, center.y + WallH / 2, center.z - WallD / 2),color},
		{vec3(center.x + WallW / 2, center.y - WallH / 2, center.z - WallD / 2),color},
		{vec3(center.x - WallW / 2, center.y - WallH / 2, center.z - WallD / 2),color},
		{vec3(center.x - WallW / 2, center.y + WallH / 2, center.z - WallD / 2),color}
	};

	// create VBO
	glGenBuffers(1, &VBO_Wall);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Wall);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_DYNAMIC_DRAW);

	// Index Buffer
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_Indeces), vertices_Indeces, GL_DYNAMIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);

}

void BindWall()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Wall);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);
}

#pragma endregion

//#pragma region Blocks
//
//
//vertex blockVertices[];
//
//void CreateBox()
//{
//	const float paddleW = 0.2, paddleH = 0.2, paddleD = 0.2;
//
//	vec3 center(0, 0, 0);
//	vec3 blockVertices[] =
//	{
//		vec3(center.x - paddleW / 2, center.y + paddleH / 2, center.z + paddleD / 2),
//		vec3(center.x - paddleW / 2, center.y - paddleH / 2, center.z + paddleD / 2),
//		vec3(center.x + paddleW / 2, center.y - paddleH / 2, center.z + paddleD / 2),
//		vec3(center.x + paddleW / 2, center.y + paddleH / 2, center.z + paddleD / 2),
//		vec3(center.x + paddleW / 2, center.y + paddleH / 2, center.z - paddleD / 2),
//		vec3(center.x + paddleW / 2, center.y - paddleH / 2, center.z - paddleD / 2),
//		vec3(center.x - paddleW / 2, center.y - paddleH / 2, center.z - paddleD / 2),
//		vec3(center.x - paddleW / 2, center.y + paddleH / 2, center.z - paddleD / 2)
//	};
//	// create VBO
//	glGenBuffers(1, &VBO_Box);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO_Paddle);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(blockVertices), blockVertices, GL_DYNAMIC_DRAW);
//
//	// Index Buffer
//	glGenBuffers(1, &IBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_Indeces), vertices_Indeces, GL_DYNAMIC_DRAW);
//
//	// shader
//	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vec3), 0);
//	glEnableVertexAttribArray(0);
//
//	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vec3), (char*)(2 * sizeof(vec3)));
//	glEnableVertexAttribArray(2);
//
//}
//
//void BindBox()
//{
//	glBindBuffer(GL_ARRAY_BUFFER, VBO_Box);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
//	glEnableVertexAttribArray(0);
//
//	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
//	glEnableVertexAttribArray(2);
//}
//
//#pragma endregion 
void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_namering, GLuint& programId)
{
	programId = InitShader(vertex_shader_file_name, fragment_shader_file_namering);
}

void UseShader(GLuint InProgramID)
{
	glUseProgram(InProgramID);
	modelMatLoc = glGetUniformLocation(InProgramID, "modelMat");
	viewMatLoc = glGetUniformLocation(InProgramID, "viewMat");
	projMatLoc = glGetUniformLocation(InProgramID, "projMat");

	glm::mat4 viewMat = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

	glm::mat4 projMat = glm::perspectiveFov(60.0f, (float)WIDTH, (float)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
}

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error";
		getchar();
		return 1;
	}
	else
	{
		if (GLEW_VERSION_3_0)
			cout << "Driver support OpenGL 3.0\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	CompileShader("VS.glsl", "FS.glsl", BasiceprogramId);
	CompileShader("VSPhong.glsl", "FSPhong.glsl", phongProgramId);
	CompileShader("VSSmooth.glsl", "FSSmooth.glsl", smoothProgramId);
	CompileShader("VSBox.glsl", "FSBox.glsl", boxProgramId);

	CreateWall();
	CreatePaddle(0);
	CreateSphere(4);
	//CreateBox();

	glClearColor(0, 0.7, 0.7, 1);
	glEnable(GL_DEPTH_TEST);

	return 0;
}

float theta = 0;
void Update()
{
	// add all tick code
	theta += 0.001f;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	UseShader(BasiceprogramId);
	BindWall();
	// draw Wall
	mat4 ModelMat = glm::translate(glm::vec3(0, 1.5, -1)) *
		glm::rotate(0.0f, glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(1, 1, 1));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));

	glDrawElements(GL_LINES, 36, GL_UNSIGNED_INT, NULL);

	BindPaddle();
	// draw Paddle
	ModelMat = glm::translate(glm::vec3(mousposX, -1, 0)) *
		glm::rotate(0.0f, glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(1, 1, 1));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	//draw blocks
	UseShader(boxProgramId);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			ModelMat = glm::translate(glm::vec3(-1.7+ j *0.7, 3.5 - i*0.5, 0)) *
				glm::rotate(0.0f, glm::vec3(1, 0, 0)) *
				glm::scale(glm::vec3(0.4, 1, 1));
			glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
			GLuint C_Location = glGetUniformLocation(boxProgramId, "c");
			glUniform1f(C_Location, c[i][j]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
		}
	}

	UseShader(smoothProgramId);
	BindSphere();
	// draw Ball
	if (firstStart)
	{
		ModelMat = glm::translate(glm::vec3(mousposX, -0.8, 0)) *
			glm::rotate(theta * 180 / 3.14f, glm::vec3(1, 1, 1)) *
			glm::scale(glm::vec3(0.1, 0.1, 0.1));
	}
	else
	{
		ModelMat = glm::translate(glm::vec3(0, -0.8, 0)) *
			glm::rotate(theta * 180 / 3.14f, glm::vec3(1, 1, 1)) *
			glm::scale(glm::vec3(0.1, 0.1, 0.1));
	}
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawArrays(GL_TRIANGLES, 0, sphere_vertices.size());

}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

	if (Init()) return 1;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				window.close();
				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				if (event.key.code == sf::Mouse::Left)
				{
					firstStart = false;
					printf("ball shoot");
				}
				break;
			}
			}
		}
		mousposX = 2 * float(sf::Mouse::getPosition(window).x) / WIDTH - 1;
		printf("%f\n", 2 * float(sf::Mouse::getPosition(window).y) / HEIGHT + 1);
		Update();
		Render();

		window.display();
	}
	return 0;
}