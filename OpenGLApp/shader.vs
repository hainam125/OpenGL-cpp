#version 330 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec4 Color;

smooth out vec4 theColor;

uniform float gScale;

void main() {
	gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0);
	theColor = Color;
}