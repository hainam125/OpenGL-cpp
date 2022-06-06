#include <stdio.h>
#include <glew.h>
#include <glut.h>
#include <string>
#include <vector>
#include <helper.h>
#include <ogldev_math_3d.h>

GLuint VBO;
GLuint IBO;
GLint gMatrixLocation;

struct Vertex
{
	Vector3f pos;
	Vector3f color;

	Vertex(){}

	Vertex(float x, float y) {
		pos = Vector3f(x, y, 0.0f);
		float r = (float)rand() / (float)RAND_MAX;
		float g = (float)rand() / (float)RAND_MAX;
		float b = (float)rand() / (float)RAND_MAX;
		color = Vector3f(r, g, b);
	}
};

void CreateVertexBuffer() {
	//rectangular
	Vertex vertices[19];
	//Center
	vertices[0] = Vertex(0.0f, 0.0f);
	//Top row
	vertices[1] = Vertex(-1.00f, 1.0f);
	vertices[2] = Vertex(-0.75f, 1.0f);
	vertices[3] = Vertex(-0.50f, 1.0f);
	vertices[4] = Vertex(-0.25f, 1.0f);
	vertices[5] = Vertex( 0.00f, 1.0f);
	vertices[6] = Vertex( 0.25f, 1.0f);
	vertices[7] = Vertex( 0.50f, 1.0f);
	vertices[8] = Vertex( 0.75f, 1.0f);
	vertices[9] = Vertex( 1.00f, 1.0f);
	//Bttom row
	vertices[10] = Vertex(-1.00f, -1.0f);
	vertices[11] = Vertex(-0.75f, -1.0f);
	vertices[12] = Vertex(-0.50f, -1.0f);
	vertices[13] = Vertex(-0.25f, -1.0f);
	vertices[14] = Vertex( 0.00f, -1.0f);
	vertices[15] = Vertex( 0.25f, -1.0f);
	vertices[16] = Vertex( 0.50f, -1.0f);
	vertices[17] = Vertex( 0.75f, -1.0f);
	vertices[18] = Vertex( 1.00f, -1.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CreateIndexBuffer() {
	unsigned int indices[] = {
		// Top triangles
		0, 2, 1,
		0, 3, 2,
		0, 4, 3,
		0, 5, 4,
		0, 6, 5,
		0, 7, 6,
		0, 8, 7,
		0, 9, 8,
		// Bottom triangles
		0, 10, 11,
		0, 11, 12,
		0, 12, 13,
		0, 13, 14,
		0, 14, 15,
		0, 15, 16,
		0, 16, 17,
		0, 17, 18,
		// Left triangle
		0, 1, 10,
		// Right triangle
		0, 18, 9
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
	static float Scale = 1.0f;

	Matrix4f Scaling(Scale, 0.0f,  0.0f,  0.0f,
                     0.0f,  Scale, 0.0f,  0.0f,
                     0.0f,  0.0f,  Scale, 0.0f,
                     0.0f,  0.0f,  0.0f,  1.0f);

	static float AngleInRadians = 0.0f;
	static float Delta = 0.0001f;

	//AngleInRadians += Delta;

	Matrix4f Rotation(cosf(AngleInRadians), -sinf(AngleInRadians), 0.0f, 0.0f,
                      sinf(AngleInRadians),  cosf(AngleInRadians), 0.0f, 0.0f,
                      0.0,                  0.0f,                  1.0f, 0.0f,
                      0.0f,                 0.0f,                  0.0f, 1.0f);

	static float Loc = 0.0f;

    Matrix4f Translation(1.0f, 0.0f, 0.0f, Loc,
                         0.0f, 1.0f, 0.0f, 0.0,
                         0.0f, 0.0f, 1.0f, 0.0,
                         0.0f, 0.0f, 0.0f, 1.0f);


	Matrix4f FinalTransform = Translation * Rotation * Scaling;
	
	//1 matrix and a row-major
	glUniformMatrix4fv(gMatrixLocation, 1, GL_TRUE, &FinalTransform.m[0][0]);
}

void RenderSceneCB() {
	glClear(GL_COLOR_BUFFER_BIT);

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

	//draw using index buffer: 18 triangle ~ 54 indices
	glDrawElements(GL_TRIANGLES, 54, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	//set seed for random func
	srand(GetCurrentProcessId());

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(1920 / 2, 1080 / 2);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("OpenGL");

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	CreateVertexBuffer();
	CreateIndexBuffer();
	InitializeProgram();

	glutDisplayFunc(RenderSceneCB);
	glutMainLoop();

	return 0;
}