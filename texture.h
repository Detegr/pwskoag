#pragma once

#include <GL3/gl3w.h>
#include <GL/glfw.h>
#include <GL3/gl3.h>
#include <string>

class C_Texture
{
	private:
		GLuint			m_Id;
		const GLvoid*	m_Data;
		unsigned int	m_Width;
		unsigned int	m_Height;
	public:
		C_Texture();
		C_Texture(unsigned int w, unsigned int h);
		bool LoadData(const std::string& path);
		void SetData(const GLvoid* data);
		void SetFiltering(GLfloat f);
		GLuint Id() const;
};
