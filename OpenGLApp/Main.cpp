#include <stdio.h>
#include <glew.h>
#include <glut.h>

void RenderSceneCB() {
	static GLclampf c = 0.0f;
	glClearColor(c, c, c, c);

	c += 1.0f / 256.0f / 50.0f;
	if (c >= 1.0f)c = 0.0f;

	//glClear is a Command while glClearColor is State Setting: which sets the color value to buffer.glClearColor is State Setting: which sets the color value to buffer.
	glClear(GL_COLOR_BUFFER_BIT);
	//force this function to be called again and again!
	glutPostRedisplay();
	//swap the role of front and back buffur from using GLUT_DOUBLE
	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	int width = 1920 / 2, height = 1080 / 2;
	glutInitWindowSize(width, height);

	int x = 200, y = 100;
	glutInitWindowPosition(x, y);

	int win = glutCreateWindow("Tutorial");
	printf("window id: %d\n", win);

	GLclampf Red = 0.0f, Green = 1.0f, Blue = 0.0f, Alpha = 0.0f;
	glClearColor(Red, Green, Blue, Alpha);

	glutDisplayFunc(RenderSceneCB);

	glutMainLoop();

	return 0;
}