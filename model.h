#pragma once
#include <iostream>
#include <vector>
#include "dtglib/Network.h"

class C_Model
{
	friend class C_ModelManager;
	public: static const unsigned short COMPONENTS_PER_VERT=2; // Change this to 3 if using 3D models.
	private:
		std::string m_Name;
		std::vector<float> m_Vertices;
		float m_Width;
		float m_Height;
		C_Model(const std::string& name, const std::vector<float>& verts, float width, float height);
	public:
		C_Model();
		C_Model(const C_Model& m);
		const std::string& M_Name() const;
		float M_Width() const;
		float M_Height() const;
		const std::vector<float>& M_Vertices() const { return m_Vertices; }
		void operator>>(dtglib::C_Packet& p) const;
};
