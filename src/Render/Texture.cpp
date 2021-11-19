#include <iostream>
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <glad/glad.h>

Texture::Texture(const char* path) 
	: m_Id{},
	m_Path{ path }
{
	load();
}

Texture::Texture() = default;

unsigned int Texture::getId() const
{
	return m_Id;
}

void Texture::load()
{
	stbi_set_flip_vertically_on_load(false);

	int width, height, channels;
	unsigned char* data{ stbi_load(m_Path, &width, &height, &channels, 0) };

	glGenTextures(1, &m_Id);
	glBindTexture(GL_TEXTURE_2D, m_Id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (data != nullptr)
	{
		int mode{ channels == 4 ? GL_RGBA : GL_RGB };
		glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture at: " << m_Path << "\n";
	}

	stbi_image_free(data);
}