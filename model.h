#pragma once
#include <GL3/gl3w.h>
#include <GL/glfw.h>
#include <GL3/gl3.h>
#include <iostream>
#include <vector>

class C_Model
{
	friend class C_ModelManager;
	public: static const unsigned short COMPONENTS_PER_VERT=2; // Change this to 3 if using 3D models.
	private:
		std::string m_Name;
		std::vector<float> m_Vertices;
		float m_Width;
		float m_Height;
	public:
		C_Model();
		C_Model(const std::string& name, std::vector<float>& verts, float width, float height);
		const std::string& M_Name() const;
		float M_Width() const;
		float M_Height() const;
		const std::vector<float>& M_Vertices() const { return m_Vertices; }
};
