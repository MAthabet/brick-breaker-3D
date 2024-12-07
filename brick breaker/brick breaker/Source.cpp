#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>

using namespace std;
using namespace glm;

enum DrawingMode
{
	Points,
	Lines,
	FilledTriangle
};

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
GLuint VBO_Triangle, VBO_Cube, IBO, BasiceprogramId;
float mousposX;
// transformation
GLuint modelMatLoc, viewMatLoc, projMatLoc;


const float paddleW = 1, paddleH = 0.2, paddleD = 0.2;
vertex paddleVertices[];

void CreatePaddle(float posX)
{
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

	// create VBO
	glGenBuffers(1, &VBO_Cube);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
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
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);
}

void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_namering, GLuint& programId)
{
	programId = InitShader(vertex_shader_file_name, fragment_shader_file_namering);
	glUseProgram(programId);
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
	
	modelMatLoc = glGetUniformLocation(BasiceprogramId, "modelMat");
	viewMatLoc = glGetUniformLocation(BasiceprogramId, "viewMat");
	projMatLoc = glGetUniformLocation(BasiceprogramId, "projMat");

	glm::mat4 viewMat = glm::lookAt(glm::vec3(0, 2, 5), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

	glm::mat4 projMat = glm::perspectiveFov(60.0f, (float)WIDTH, (float)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

	CreatePaddle(0);
	glClearColor(0, 0.5, 0.5, 1);
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

	BindPaddle();
	// draw cube
	mat4 ModelMat = glm::translate(glm::vec3(mousposX, 0, 0)) *
		glm::rotate(0.0f, glm::vec3(1, 0, 0)) *
		glm::scale(glm::vec3(1, 1, 1));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

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
			case sf::Event::KeyPressed:
			{
				/*if (event.key.code == sf::Keyboard::Num1)
				{
					Current_DrawingMode = DrawingMode::Points;
				}
				if (event.key.code == sf::Keyboard::Num2)
				{
					Current_DrawingMode = DrawingMode::Lines;
				}
				if (event.key.code == sf::Keyboard::Num3)
				{
					Current_DrawingMode = DrawingMode::FilledTriangle;
				}*/
				break;
			}
			}
		}
		mousposX = 2 * float(sf::Mouse::getPosition(window).x) / WIDTH - 1;
		Update();
		Render();

		window.display();
	}
	return 0;
}