#include <stdio.h>
#include <glew.h>
#include <glut.h>
#include <string>
#include <vector>
#include <helper.h>
#include <ogldev_math_3d.h>

GLuint VBO;
GLint gMatrixLocation;

void CreateVertexBuffer() {
	float vertexData[] = {
		-1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	static float Scale = 0.25f;

	Matrix4f Scaling(Scale, 0.0f,  0.0f,  0.0f,
                     0.0f,  Scale, 0.0f,  0.0f,
                     0.0f,  0.0f,  Scale, 0.0f,
                     0.0f,  0.0f,  0.0f,  1.0f);

	static float AngleInRadians = 0.0f;
	static float Delta = 0.0001f;

	AngleInRadians += Delta;

	Matrix4f Rotation(cosf(AngleInRadians), -sinf(AngleInRadians), 0.0f, 0.0f,
                      sinf(AngleInRadians), cosf(AngleInRadians),  0.0f, 0.0f,
                      0.0,                  0.0f,                  1.0f, 0.0f,
                      0.0f,                 0.0f,                  0.0f, 1.0f);

	static float Loc = 0.5f;

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

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(1080 / 2, 1080 / 2);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("OpenGL");

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
	InitializeProgram();

	glutDisplayFunc(RenderSceneCB);
	glutMainLoop();

	return 0;
}