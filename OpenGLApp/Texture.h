#pragma once
#include <string>
#include <glew.h>

class Texture
{
public:
	Texture(GLenum textureTarget, const std::string& fileName);
	//should be called one to load the texture
	bool Load();
	//must be called at least one for the specification unit
	void Bind(GLenum textureUnit);

	void GetImageSize(int& imageWidth, int& imageHeight) {
		imageWidth = m_imageWidth;
		imageHeight = m_imageHeight;
	}

private:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	int m_imageWidth = 0;
	int m_imageHeight = 0;
	int m_imageBPP = 0;
};