#include "modelmanager.h"
#include "filereader.h"
#include <sstream>
#include <iostream>
#include <stdexcept>

C_ModelManager::~C_ModelManager()
{
	std::cout << "Destroying modelmanager..." << std::flush;
	for(std::vector<C_Model*>::iterator it=m_Models.begin(); it!=m_Models.end(); ++it)
	{
		delete *it;
	}
	std::cout << "OK!" << std::endl;
}

bool C_ModelManager::M_Load(const std::string& name, const std::string& path)
{
	std::vector<std::string> f=C_FileReader::M_ReadToArray(path);
	std::stringstream ss;
	ss.precision(3); // Increase if more precision is needed.

	float val=0;
	float xmin,ymin,xmax,ymax;
	xmin=ymin=2.0f;
	xmax=ymax=-2.0f;
	unsigned char i=0;
	std::vector<float> verts;
	for(std::vector<std::string>::iterator it=f.begin(); it!=f.end(); ++it, ++i)
	{
		if(it->find(',') != std::string::npos) continue;
		ss.clear();
		ss << *it;
		ss >> val;
		if(i)
		{
			if(ymin>val) ymin=val;
			else if(ymax<val) ymax=val;
			i=-1;
		}
		else
		{
			if(xmin>val) xmin=val;
			else if(xmax<val) xmax=val;
		}
		verts.push_back(val);
	}

	/*
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*verts.size(), &verts[0], GL_STATIC_DRAW);
	*/

	m_Models.push_back(new C_Model(name, verts, xmax-xmin, ymax-ymin));

	return true;
}

C_Model* C_ModelManager::M_Get(const std::string& name) const
{
	for(std::vector<C_Model*>::const_iterator it=m_Models.begin(); it!=m_Models.end(); ++it)
	{
		if((*it)->M_Name() == name) return *it;
	}
	throw std::runtime_error("Model " + name + " does not exist!");
}