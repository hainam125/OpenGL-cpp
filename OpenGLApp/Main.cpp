#include <stdio.h>
#include <glew.h>
#include <glut.h>
#include <string>
#include <vector>
#include <helper.h>
#include <ogldev_math_3d.h>

#include "camera.h"
#include "WorldTransform.h"

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 540

GLuint VBO;
GLuint IBO;
GLint gMatrixLocation;

WorldTransform CubeTransform;
Vector3f CameraPos(0.0f, 0.0f, -1.0f);
Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
Vector3f CameraUp(0.0f, 1.0f, 0.0f);
Camera MainCamera(WINDOW_WIDTH, WINDOW_HEIGHT, CameraPos, CameraTarget, CameraUp);

float FOV = 90.0f;
float nearZ = 1.0f;
float farZ = 10.0f;
PersProjInfo persProjInfo = { FOV, WINDOW_WIDTH, WINDOW_HEIGHT, nearZ, farZ };

struct Vertex
{
	Vector3f pos;
	Vector3f color;

	Vertex(){}

	Vertex(float x, float y, float z) {
		pos = Vector3f(x, y, z);
		float r = (float)rand() / (float)RAND_MAX;
		float g = (float)rand() / (float)RAND_MAX;
		float b = (float)rand() / (float)RAND_MAX;
		color = Vector3f(r, g, b);
	}
};

void CreateVertexBuffer() {
	Vertex vertices[8];

	vertices[0] = Vertex( 0.5f,  0.5f,  0.5f);
	vertices[1] = Vertex(-0.5f,  0.5f, -0.5f);
	vertices[2] = Vertex(-0.5f,  0.5f,  0.5f);
	vertices[3] = Vertex( 0.5f, -0.5f, -0.5f);
	vertices[4] = Vertex(-0.5f, -0.5f, -0.5f);
	vertices[5] = Vertex( 0.5f,  0.5f, -0.5f);
	vertices[6] = Vertex( 0.5f, -0.5f,  0.5f);
	vertices[7] = Vertex(-0.5f, -0.5f,  0.5f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CreateIndexBuffer() {
	unsigned int indices[] = {
		0, 1, 2,
		1, 3, 4,
		5, 6, 3,
		7, 3, 6,
		2, 4, 7,
		0, 7, 6,
		0, 5, 1,
		1, 5, 3,
		5, 0, 6,
		7, 4, 3,
		2, 1, 4,
		0, 2, 7
	};

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile) {
	const char* strFileData = strShaderFile.c_str();

	GLuint shader = glCreateShader(eShaderType);
	//set data for shader object
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* strInfoLog = new GLchar[(LONG64)infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char* strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
		exit(1);
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint>& shaderList) {
	GLuint program = glCreateProgram();
	for (size_t i = 0; i < shaderList.size(); i++) {
		glAttachShader(program, shaderList[i]);
	}
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* strInfoLog = new GLchar[(LONG64)infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Link failure: %s\n", strInfoLog);
		delete[]strInfoLog;
		exit(1);
	}

	//compiler will allocate the indices of the uniform at link time
	gMatrixLocation = glGetUniformLocation(program, "gMatrix");
	if (gMatrixLocation == -1) {
		fprintf(stderr, "Error getting uniform location 'gTranslation'\n");
		exit(1);
	}

	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* strInfoLog = new GLchar[(LONG64)infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Validate failure: %s\n", strInfoLog);
		delete[]strInfoLog;
		exit(1);
	}

	for (size_t i = 0; i < shaderList.size(); i++) {
		glDetachShader(program, shaderList[i]);
	}

	return program;
}

void InitializeProgram() {
	std::vector<GLuint> shaderList;
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, get_file_contents("shader.vs")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, get_file_contents("shader.fs")));

	GLint program = CreateProgram(shaderList);
	for (size_t i = 0; i < shaderList.size(); i++) {
		glDeleteShader(shaderList[i]);
	}
	glUseProgram(program);
}

void Transformation() {
	CubeTransform.SetPosition(0.0f, 0.f, 2.5f);
	CubeTransform.Rotate(0.0f, 0.05f, 0.0f);
	Matrix4f World = CubeTransform.GetMatrix();

	Matrix4f Camera = MainCamera.GetMatrix();

	Matrix4f Projection;
	Projection.InitPersProjTransform(persProjInfo);
	Matrix4f FinalTransform = Projection * Camera * World;
	
	//1 matrix and a row-major
	glUniformMatrix4fv(gMatrixLocation, 1, GL_TRUE, &FinalTransform.m[0][0]);
}

void RenderSceneCB() {
	glClear(GL_COLOR_BUFFER_BIT);

	MainCamera.OnRender();

	Transformation();

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	//position
	glEnableVertexAttribArray(0);
	//stride: 3 floats of position + 3 floats of color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

	//color
	glEnableVertexAttribArray(1);
	//offset: start after 3 floats of position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	//draw using index buffer: 12 triangle ~ 36 indices
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
	glutPostRedisplay();
}

void KeyboardCB(unsigned char key, int mouseX, int mouseY) {
	MainCamera.OnKeyBoard(key);
}

void SpecialKeyboardCB(int key, int mouseX, int mouseY) {
	MainCamera.OnKeyBoard(key);
}

void PassiveMouseCB(int x, int y)
{
	MainCamera.OnMouse(x, y);
}

void initializeGlutCallbacks() {
	glutDisplayFunc(RenderSceneCB);
	glutKeyboardFunc(KeyboardCB);
	glutSpecialFunc(SpecialKeyboardCB);
	glutPassiveMotionFunc(PassiveMouseCB);
}

int main(int argc, char** argv) {
	//set seed for random func
	srand(GetCurrentProcessId());

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("OpenGL");

	char game_mode_string[64];
	// Game mode string example: <Width>x<Height>@<FPS>
	snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", WINDOW_WIDTH, WINDOW_HEIGHT);
	glutGameModeString(game_mode_string);
	glutEnterGameMode();

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	CreateVertexBuffer();
	CreateIndexBuffer();
	InitializeProgram();

	initializeGlutCallbacks();
	glutMainLoop();

	return 0;
}