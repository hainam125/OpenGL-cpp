#include <stdio.h>
#include <glew.h>
#include <glut.h>
#include <ogldev_math_3d.h>

GLuint bufID;

void CreateVertexBuffer() {
	Vector3f Vertices[3];
	Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
	Vertices[1] = Vector3f( 0.0f,  1.0f, 0.0f);
	Vertices[2] = Vector3f( 1.0f, -1.0f, 0.0f);

	glGenBuffers(1, &bufID);
	glBindBuffer(GL_ARRAY_BUFFER, bufID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderSceneCB() {
	glClear(GL_COLOR_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, bufID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}

int main(int argc, char** argv) {
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
	glFrontFace(GL_CW);
	//glCullFace(GL_FRONT);

	CreateVertexBuffer();

	glutDisplayFunc(RenderSceneCB);
	glutMainLoop();

	return 0;
}