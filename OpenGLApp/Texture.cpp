#include "Texture.h"
#include <stb_image.h>

Texture::Texture(GLenum textureTarget, const std::string& fileName) {
	m_textureTarget = textureTarget;
	m_fileName = fileName;
}

bool Texture::Load() {
	stbi_set_flip_vertically_on_load(1);

	unsigned char* image_data = stbi_load(m_fileName.c_str(), &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

	if (!image_data) {
		printf("Can't load texture from '%s' - %s\n", m_fileName.c_str(), stbi_failure_reason());
		exit(0);
	}
	printf("Width %d, height %d, bpp %d\n", m_imageWidth, m_imageHeight, m_imageBPP);

	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);
	if (m_textureTarget == GL_TEXTURE_2D) {
		glTexImage2D(m_textureTarget, 0, GL_RGB, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	}
	else
	{
		printf("Support for texture target %x is not implemented\n", m_textureTarget);
		exit(1);
	}

	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(m_textureTarget, 0);

	stbi_image_free(image_data);

	return true;
}

//activates the texture unit and bind texture object to it
void Texture::Bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}