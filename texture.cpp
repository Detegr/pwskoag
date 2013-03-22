#include "texture.h"
#include <iostream>

C_Texture::C_Texture() : m_Id(0), m_Data(0), m_Width(0), m_Height(0)
{
}

C_Texture::C_Texture(unsigned int w, unsigned int h) : m_Id(0), m_Data(NULL), m_Width(w), m_Height(h)
{
	glGenTextures(1, &m_Id);
}

void C_Texture::SetData(const GLvoid* data)
{
	glBindTexture(GL_TEXTURE_2D, m_Id);
	m_Data=data;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_Data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool C_Texture::LoadData(const std::string& path)
{
	if(!m_Id)
	{
		glGenTextures(1, &m_Id);
		glBindTexture(GL_TEXTURE_2D, m_Id);
	}
	bool ret=glfwLoadTexture2D(path.c_str(), 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return ret;
}

void C_Texture::SetFiltering(GLfloat f)
{
	glBindTexture(GL_TEXTURE_2D, m_Id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, f);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint C_Texture::Id() const
{
	return m_Id;
}
