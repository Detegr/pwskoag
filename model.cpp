#include "model.h"

C_Model::C_Model() : m_Name(), m_Vertices(), m_Width(0), m_Height(0) {}
C_Model::C_Model(const std::string& name, std::vector<float>& verts, float width, float height) :
	m_Name(name), m_Vertices(verts), m_Width(width), m_Height(height) {}

/*
std::pair<GLuint,unsigned short> C_Model::M_Get() const
{
	return std::make_pair(m_Vbo, m_Vertices);
}
*/

/*
void C_Model::M_Draw() const
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glVertexAttribPointer(0,C_Model::COMPONENTS_PER_VERT,GL_FLOAT,GL_FALSE,0,(void*)0);
	glDrawArrays(GL_TRIANGLE_STRIP,0,m_Vertices.size());
	glDisableVertexAttribArray(0);
}
*/

const std::string& C_Model::M_Name() const
{
	return m_Name;
}

float C_Model::M_Width() const
{
	return m_Width;
}

float C_Model::M_Height() const
{
	return m_Height;
}
