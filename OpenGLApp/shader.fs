#version 330 core

smooth in vec4 theColor;

out vec4 FragColor;

void main() {
	FragColor = theColor;
}