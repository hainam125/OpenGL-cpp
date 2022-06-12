#include <stdio.h>
#include <glew.h>
#include <glut.h>
#include <freeglut.h>
#include <string>
#include <vector>
#include <helper.h>
#include <ogldev_math_3d.h>

#include "camera.h"
#include "WorldTransform.h"
#include "Texture.h"

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 540

struct Vertex
{
	Vector3f pos;
	Vector2f tex;

	Vertex(){}

	Vertex(const Vector3f& _pos, const Vector2f& _tex) {
		pos = _pos;
		tex = _tex;
	}
};

class OpenGLTutorial
{
public:
	OpenGLTutorial();
	~OpenGLTutorial();

	bool Init();
	void RenderSceneCB();
	void KeyboardCB(unsigned char key, int mouseX, int mouseY);
	void SpecialKeyboardCB(int key, int mouseX, int mouseY);
	void PassiveMouseCB(int x, int y);

private:
	void CreateCubeVAO();
	void CreatePyramidVAO();
	GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile);
	GLuint CreateProgram(const std::vector<GLuint>& shaderList);
	void InitializeProgram();
	void Transformation();

	GLuint CubeVAO = -1;
	GLuint CubeVBO = -1;
	GLuint CubeIBO = -1;

	GLuint PyramidVAO = -1;
	GLuint PyramidVBO = -1;
	GLuint PyramidIBO = -1;

	GLint gMatrixLocation;
	GLint gSamplerLocation;
	Texture* pTexture = NULL;
	Camera* MainCamera = NULL;
	WorldTransform Transform;
	PersProjInfo persProjInfo;
};

OpenGLTutorial::OpenGLTutorial()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	float FOV = 90.0f;
	float nearZ = 1.0f;
	float farZ = 10.0f;
	persProjInfo = { FOV, WINDOW_WIDTH, WINDOW_HEIGHT, nearZ, farZ };
}

OpenGLTutorial::~OpenGLTutorial()
{
	if (pTexture) {
		delete pTexture;
	}

	if (MainCamera) {
		delete MainCamera;
	}

	if (CubeVAO != -1) {
		glDeleteVertexArrays(1, &CubeVAO);
	}

	if (CubeVBO != -1) {
		glDeleteBuffers(1, &CubeVBO);
	}

	if (CubeIBO != -1) {
		glDeleteBuffers(1, &CubeIBO);
	}

	if (PyramidVAO != -1) {
		glDeleteVertexArrays(1, &PyramidVAO);
	}

	if (PyramidVBO != -1) {
		glDeleteBuffers(1, &PyramidVBO);
	}

	if (PyramidIBO != -1) {
		glDeleteBuffers(1, &PyramidIBO);
	}
}

bool OpenGLTutorial::Init() {
	CreateCubeVAO();
	CreatePyramidVAO();

	//bind a default object
	glBindVertexArray(CubeVAO);

	InitializeProgram();

	pTexture = new Texture(GL_TEXTURE_2D, "../Content/bricks.jpg");
	if (!pTexture->Load()) {
		return false;
	}

	pTexture->Bind(GL_TEXTURE0);
	glUniform1i(gSamplerLocation, 0);

	Vector3f CameraPos(0.0f, 0.0f, -1.0f);
	Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
	Vector3f CameraUp(0.0f, 1.0f, 0.0f);
	MainCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, CameraPos, CameraTarget, CameraUp);

	return true;
}

void OpenGLTutorial::CreateCubeVAO() {
	glGenVertexArrays(1, &CubeVAO);
	glBindVertexArray(CubeVAO);

	//CreateVertexBuffer
	Vertex vertices[8];

	Vector2f t00 = Vector2f(0.0f, 0.0f);  // Bottom left
	Vector2f t01 = Vector2f(0.0f, 1.0f);  // Top left
	Vector2f t10 = Vector2f(1.0f, 0.0f);  // Bottom right
	Vector2f t11 = Vector2f(1.0f, 1.0f);  // Top right

	vertices[0] = Vertex(Vector3f( 0.5f,  0.5f,  0.5f), t00);
	vertices[1] = Vertex(Vector3f(-0.5f,  0.5f, -0.5f), t01);
	vertices[2] = Vertex(Vector3f(-0.5f,  0.5f,  0.5f), t10);
	vertices[3] = Vertex(Vector3f( 0.5f, -0.5f, -0.5f), t11);
	vertices[4] = Vertex(Vector3f(-0.5f, -0.5f, -0.5f), t00);
	vertices[5] = Vertex(Vector3f( 0.5f,  0.5f, -0.5f), t10);
	vertices[6] = Vertex(Vector3f( 0.5f, -0.5f,  0.5f), t01);
	vertices[7] = Vertex(Vector3f(-0.5f, -0.5f,  0.5f), t11);

	glGenBuffers(1, &CubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

	//CreateIndexBuffer
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

	glGenBuffers(1, &CubeIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLTutorial::CreatePyramidVAO()
{
	glGenVertexArrays(1, &PyramidVAO);
	glBindVertexArray(PyramidVAO);

	Vector2f t00  = Vector2f(0.0f, 0.0f);
	Vector2f t050 = Vector2f(0.5f, 0.0f);
	Vector2f t10  = Vector2f(1.0f, 0.0f);
	Vector2f t051 = Vector2f(0.5f, 1.0f);

	Vertex Vertices[4] = { Vertex(Vector3f(-1.0f, -1.0f,  0.5773f ), t00),
						   Vertex(Vector3f( 0.0f, -1.0f, -1.15475f), t050),
						   Vertex(Vector3f( 1.0f, -1.0f,  0.5773f ), t10),
						   Vertex(Vector3f( 0.0f,  1.0f,  0.0f    ), t051) };

	glGenBuffers(1, &PyramidVBO);
	glBindBuffer(GL_ARRAY_BUFFER, PyramidVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// tex coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

	unsigned int Indices[] = { 0, 3, 1,
							   1, 3, 2,
							   2, 3, 0,
							   0, 1, 2 };

	glGenBuffers(1, &PyramidIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PyramidIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint OpenGLTutorial::CreateShader(GLenum eShaderType, const std::string& strShaderFile) {
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

GLuint OpenGLTutorial::CreateProgram(const std::vector<GLuint>& shaderList) {
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
		fprintf(stderr, "Error getting uniform location 'gMatrix'\n");
		exit(1);
	}

	gSamplerLocation = glGetUniformLocation(program, "gSampler");
	if (gSamplerLocation == -1) {
		printf("Error getting uniform location of 'gSampler'\n");
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

void OpenGLTutorial::InitializeProgram() {
	std::vector<GLuint> shaderList;
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, get_file_contents("shader.vs")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, get_file_contents("shader.fs")));

	GLint program = CreateProgram(shaderList);
	for (size_t i = 0; i < shaderList.size(); i++) {
		glDeleteShader(shaderList[i]);
	}
	glUseProgram(program);
}

void OpenGLTutorial::Transformation() {
	Transform.SetPosition(0.0f, 0.f, 2.5f);
	Transform.Rotate(0.0f, 0.05f, 0.0f);
	Matrix4f World = Transform.GetMatrix();

	Matrix4f Camera = MainCamera->GetMatrix();

	Matrix4f Projection;
	Projection.InitPersProjTransform(persProjInfo);
	Matrix4f FinalTransform = Projection * Camera * World;
	
	//1 matrix and a row-major
	glUniformMatrix4fv(gMatrixLocation, 1, GL_TRUE, &FinalTransform.m[0][0]);
}

void OpenGLTutorial::RenderSceneCB() {
	glClear(GL_COLOR_BUFFER_BIT);

	MainCamera->OnRender();

	Transformation();

	GLint currentVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);

	if (currentVAO == CubeVAO) {
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void OpenGLTutorial::KeyboardCB(unsigned char key, int mouseX, int mouseY) {
	switch (key)
	{
	case 'q':
	case 27://escape key code
		exit(0);
	case '1':
		glBindVertexArray(CubeVAO);
		break;
	case '2':
		glBindVertexArray(PyramidVAO);
		break;
	}
	MainCamera->OnKeyBoard(key);
}

void OpenGLTutorial::SpecialKeyboardCB(int key, int mouseX, int mouseY) {
	MainCamera->OnKeyBoard(key);
}

void OpenGLTutorial::PassiveMouseCB(int x, int y)
{
	MainCamera->OnMouse(x, y);
}


OpenGLTutorial* tutorial = NULL;

void RenderSceneCB() {
	tutorial->RenderSceneCB();
}

void KeyboardCB(unsigned char key, int mouseX, int mouseY) {
	tutorial->KeyboardCB(key, mouseX, mouseY);
}

void SpecialKeyboardCB(int key, int mouseX, int mouseY) {
	tutorial->SpecialKeyboardCB(key, mouseX, mouseY);
}

void PassiveMouseCB(int x, int y)
{
	tutorial->PassiveMouseCB(x, y);
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
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("OpenGL");

	/*char game_mode_string[64];
	// Game mode string example: <Width>x<Height>@<FPS>
	snprintf(game_mode_string, sizeof(game_mode_string), "%dx%d@60", WINDOW_WIDTH, WINDOW_HEIGHT);
	glutGameModeString(game_mode_string);
	glutEnterGameMode();*/

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		return 1;
	}

	initializeGlutCallbacks();

	tutorial = new OpenGLTutorial();
	if (!tutorial->Init()) {
		return 1;
	}

	glutMainLoop();

	return 0;
}