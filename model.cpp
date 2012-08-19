#include "model.h"
#include "networkenum.h"

C_Model::C_Model() : m_Name(), m_Vertices(), m_Width(0), m_Height(0) {}
C_Model::C_Model(const std::string& name, const std::vector<float>& verts, float width, float height) :
	m_Name(name), m_Vertices(verts), m_Width(width), m_Height(height) {}

C_Model::C_Model(const C_Model& m)
{
	if(this!=&m)
	{
		this->m_Name=m.m_Name;
		this->m_Vertices=m.m_Vertices;
		this->m_Width=m.m_Width;
		this->m_Height=m.m_Height;
	}
}

void C_Model::operator>>(dtglib::C_Packet& p) const
{
	p << NET::ModelBegin;
	p << m_Name;
	for(std::vector<float>::const_iterator it=m_Vertices.begin(); it!=m_Vertices.end(); ++it)
	{
		p << NET::ModelIndex;
		p << *it;
	}
	p << NET::ModelDimensions;
	p << m_Width;
	p << m_Height;
}

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
