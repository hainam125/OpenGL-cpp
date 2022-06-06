#version 330 core

layout (location = 0) in vec3 Position;

out vec4 Color;

const vec4 colors[3] = vec4[3](
	vec4(1,0,0,1),
	vec4(0,1,0,1),
	vec4(0,0,1,1)
);

uniform mat4 gMatrix;

void main() {
	gl_Position = gMatrix * vec4(Position,1.0);
	Color = colors[gl_VertexID];
}