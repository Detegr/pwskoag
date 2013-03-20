#include "texture.h"
#include <iostream>

C_Texture::C_Texture() : m_Id(0), m_Data(0), m_Width(0), m_Height(0)
{
}

C_Texture::C_Texture(unsigned int w, unsigned int h) : m_Id(0), m_Data(NULL), m_Width(w), m_Height(h)
{
	glGenTextures(1, &m_Id);
	glBindTexture(GL_TEXTURE_2D, m_Id);
}

void C_Texture::SetData(const GLvoid* data)
{
	m_Data=data;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_Data);
}

bool C_Texture::LoadData(const std::string& path)
{
	if(!m_Id)
	{
		glGenTextures(1, &m_Id);
		glBindTexture(GL_TEXTURE_2D, m_Id);
	}
	return glfwLoadTexture2D(path.c_str(), 0);
}

void C_Texture::SetFiltering(GLfloat f)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, f);
}

GLuint C_Texture::Id() const
{
	return m_Id;
}
