#version 330 core

in vec2 TexCoord0;

out vec4 FragColor;

//texture unit whose index is configured into the sampler
uniform sampler2D gSampler;

void main() {
	FragColor = texture2D(gSampler, TexCoord0);
}