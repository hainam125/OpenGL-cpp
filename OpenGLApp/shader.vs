#version 330 core

layout (location = 0) in vec4 Position;

uniform mat4 gMatrix;

void main() {
	gl_Position = gMatrix * Position;
}