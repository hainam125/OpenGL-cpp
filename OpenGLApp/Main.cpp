#include <stdio.h>
#include <glew.h>
#include <glut.h>
#include <ogldev_math_3d.h>

GLuint bufID;

//Creating VBOs
void CreateVertexBuffer() {
	Vector3f Vertices[3];
	Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
	Vertices[1] = Vector3f( 0.0f,  1.0f, 0.0f);
	Vertices[2] = Vector3f( 1.0f, -1.0f, 0.0f);

	//generating an unique identifier
	//creates the buffer object, storing the handle to the object in the variable bufID
	glGenBuffers(1, &bufID);
	printf("buffer id: %d\n", bufID);

	//tell the OpenGL driver that we plan to populate it with vertex attribute
	//binds the newly-created buffer object to the GL_ARRAY_BUFFER binding target
	glBindBuffer(GL_ARRAY_BUFFER, bufID);
	//allocates memory for the buffer currently bound to GL_ARRAY_BUFFER
	//transfer the vertex data (the position in this case) into the vertex buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	//unbound, cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderSceneCB() {
	glClear(GL_COLOR_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, bufID);

	//allow vertex attribute at index 0 to be modified
	//layout(location = 0) in vec4 position;//code in vertex shader
	glEnableVertexAttribArray(0);

	//define an array of generic vertex attr at index 0 with 3 element of FLOATs
	//without normalize data to [0,1] or [-1,1], without offset between consecutive
	//generic vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Rendering using VBOs
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

	//swap the role of front and back buffer from using GLUT_DOUBLE
	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	int width = 1920 / 2, height = 1080 / 2;
	glutInitWindowSize(width, height);

	int x = 200, y = 100;
	glutInitWindowPosition(x, y);

	int win = glutCreateWindow("Tutorial 02");
	printf("window id: %d\n", win);

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
		return 1;
	}

	GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
	glClearColor(Red, Green, Blue, Alpha);

	//Setup face
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	//glCullFace(GL_FRONT);

	CreateVertexBuffer();

	glutDisplayFunc(RenderSceneCB);

	glutMainLoop();

	return 0;
}